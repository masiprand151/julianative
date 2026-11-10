declare const process: any;

import { FlatListProps, VNode } from "./types";
import { dispatchEvent } from "./components/Button";
import { dispatchFloatingActionButtonPress } from "./components/FloatingActionButton";
import { dispatchModalClose } from "./components/Modal";
import { inputDispatchChange } from "./components/TextInput";
import { dispatchTableResize, dispatchTableSort } from "./components/Table";
import { dispatchWidgetEvent } from "./components/Widget";
import { requestRender, withStateScope } from "./state";
import { handleStorageResponse } from "./localStorage";
import { Application } from "./Application";
import { handleNativeResponse } from "./NativeBridge";
import { checkboxDispatchChange } from "./components";

type ListViewport = { offset: number; viewportSize: number };
type VirtualListConfig = {
  keys: string[];
  totalCount: number;
  itemSize: number;
  gap: number;
  initialNumToRender: number;
  overscan: number;
  scrollable: boolean;
};

const virtualListViewports = new Map<string, ListViewport>();
const virtualListConfigs = new Map<string, VirtualListConfig>();
const virtualListItemSizes = new Map<string, Map<string, number>>();
let activeVirtualListIds = new Set<string>();

function nodeIdForKey(parentId: string, key: string | number): string {
  const encoded = encodeURIComponent(JSON.stringify(key)).replace(/\./g, "%2E");
  return `${parentId}.key.${encoded}`;
}

function serializeChildren(children: VNode[], nodeId: string): any[] {
  const seenKeys = new Set<string>();
  return children.map((child, index) => {
    if (child.key === undefined) {
      return serialize(child, `${nodeId}.${index}`);
    }

    const key = JSON.stringify(child.key);
    if (seenKeys.has(key)) {
      throw new Error(`Duplicate sibling key "${String(child.key)}".`);
    }
    seenKeys.add(key);
    return serialize(child, nodeIdForKey(nodeId, child.key));
  });
}

function listWindow(
  config: VirtualListConfig,
  viewport: ListViewport,
  measuredSizes: Map<string, number>,
): {
  start: number;
  end: number;
  leadingExtent: number;
  trailingExtent: number;
} {
  if (config.totalCount === 0) {
    return { start: 0, end: 0, leadingExtent: 0, trailingExtent: 0 };
  }
  const sizes = config.keys.map(
    (key) => measuredSizes.get(key) ?? config.itemSize,
  );
  const starts: number[] = [];
  let totalExtent = 0;
  for (let index = 0; index < sizes.length; index++) {
    starts.push(totalExtent);
    totalExtent += sizes[index];
    if (index < sizes.length - 1) totalExtent += config.gap;
  }

  if (!config.scrollable) {
    return {
      start: 0,
      end: config.totalCount,
      leadingExtent: 0,
      trailingExtent: 0,
    };
  }
  if (viewport.viewportSize <= 0) {
    const end = Math.min(config.totalCount, config.initialNumToRender);
    const trailingItemCount = config.totalCount - end;
    return {
      start: 0,
      end,
      leadingExtent: 0,
      trailingExtent:
        sizes.slice(end).reduce((sum, size) => sum + size, 0) +
        config.gap * Math.max(0, trailingItemCount - (end > 0 ? 1 : 0)),
    };
  }

  const offset = Math.min(
    Math.max(0, viewport.offset),
    Math.max(0, totalExtent - 1),
  );
  let firstVisible = 0;
  while (
    firstVisible + 1 < config.totalCount &&
    starts[firstVisible + 1] <= offset
  ) {
    firstVisible++;
  }
  const viewportEnd = offset + Math.max(1, viewport.viewportSize);
  let visibleEnd = firstVisible + 1;
  while (visibleEnd < config.totalCount && starts[visibleEnd] < viewportEnd) {
    visibleEnd++;
  }
  const start = Math.max(0, firstVisible - config.overscan);
  const end = Math.min(
    config.totalCount,
    Math.max(start + config.initialNumToRender, visibleEnd + config.overscan),
  );
  const leadingExtent = sizes
    .slice(0, start)
    .reduce((sum, size) => sum + size, 0);
  const trailingExtent =
    sizes.slice(end).reduce((sum, size) => sum + size, 0) +
    config.gap *
      Math.max(0, config.totalCount - end - (end < config.totalCount ? 1 : 0));
  return { start, end, leadingExtent, trailingExtent };
}

function virtualListGap(value: unknown): number {
  if (typeof value === "number") {
    return Number.isFinite(value) ? Math.max(0, Math.trunc(value)) : 0;
  }
  if (typeof value !== "string") return 0;

  const match = value.trim().match(/^(-?\d+(?:\.\d+)?)(px|pt)$/);
  if (!match) return 0;
  const amount = Number(match[1]);
  const scale = match[2] === "pt" ? 96 / 72 : 1;
  return Math.max(0, Math.round(amount * scale));
}

function updateListViewport(
  nodeId: string,
  offset: number,
  viewportSize: number,
  items: Array<{ index: number; size: number }>,
): void {
  const config = virtualListConfigs.get(nodeId);
  if (!config) return;

  const measuredSizes =
    virtualListItemSizes.get(nodeId) ?? new Map<string, number>();
  const previousViewport = virtualListViewports.get(nodeId) ?? {
    offset: 0,
    viewportSize: 0,
  };
  const nextViewport = {
    offset: Math.max(0, offset),
    viewportSize: Math.max(0, viewportSize),
  };
  const previousWindow = listWindow(config, previousViewport, measuredSizes);
  let measurementsChanged = false;
  for (const item of items) {
    if (
      !Number.isInteger(item.index) ||
      item.index < 0 ||
      item.index >= config.totalCount ||
      !Number.isFinite(item.size) ||
      item.size <= 0
    ) {
      continue;
    }
    const key = config.keys[item.index];
    const size = Math.round(item.size);
    if (measuredSizes.get(key) !== size) {
      measuredSizes.set(key, size);
      measurementsChanged = true;
    }
  }
  if (measurementsChanged) virtualListItemSizes.set(nodeId, measuredSizes);
  const nextWindow = listWindow(config, nextViewport, measuredSizes);
  virtualListViewports.set(nodeId, nextViewport);
  if (
    measurementsChanged ||
    previousWindow.start !== nextWindow.start ||
    previousWindow.end !== nextWindow.end
  ) {
    requestRender();
  }
}

function serializeFlatList(node: VNode, nodeId: string): any {
  const props = node.props as FlatListProps<any>;
  const data = props.data;
  const horizontal = props.horizontal ?? false;
  const scrollable = props.scrollEnabled ?? true;
  const itemSize = props.estimatedItemSize ?? 56;
  const initialNumToRender = props.initialNumToRender ?? 12;
  const overscan = props.overscan ?? 3;
  if (!Number.isFinite(itemSize) || itemSize <= 0) {
    throw new RangeError(
      "FlatList estimatedItemSize must be a positive number.",
    );
  }
  if (!Number.isInteger(initialNumToRender) || initialNumToRender < 1) {
    throw new RangeError(
      "FlatList initialNumToRender must be a positive integer.",
    );
  }
  if (!Number.isInteger(overscan) || overscan < 0) {
    throw new RangeError("FlatList overscan must be a non-negative integer.");
  }
  const contentStyle = props.contentContainerStyle ?? {};
  const gap = virtualListGap(contentStyle.gap);
  const keys = data.map((item, index) =>
    props.keyExtractor ? props.keyExtractor(item, index) : index,
  );
  const uniqueKeys = new Set<string>();
  for (const key of keys) {
    if (typeof key !== "string" && typeof key !== "number") {
      throw new Error("FlatList keyExtractor must return a string or number.");
    }
    if (typeof key === "number" && !Number.isFinite(key)) {
      throw new Error("FlatList numeric keys must be finite.");
    }
    const serializedKey = JSON.stringify(key);
    if (uniqueKeys.has(serializedKey)) {
      throw new Error(`FlatList contains duplicate key "${String(key)}".`);
    }
    uniqueKeys.add(serializedKey);
  }

  activeVirtualListIds.add(nodeId);
  let measuredSizes = virtualListItemSizes.get(nodeId);
  if (!measuredSizes) {
    measuredSizes = new Map<string, number>();
    virtualListItemSizes.set(nodeId, measuredSizes);
  }
  for (const key of measuredSizes.keys()) {
    if (!uniqueKeys.has(key)) measuredSizes.delete(key);
  }
  const viewport = virtualListViewports.get(nodeId) ?? {
    offset: 0,
    viewportSize: 0,
  };
  const config: VirtualListConfig = {
    keys: keys.map((key) => JSON.stringify(key)),
    totalCount: data.length,
    itemSize,
    gap,
    initialNumToRender,
    overscan,
    scrollable,
  };
  virtualListConfigs.set(nodeId, config);
  const { start, end, leadingExtent, trailingExtent } = listWindow(
    config,
    viewport,
    measuredSizes,
  );
  const children: VNode[] = [];

  if (data.length === 0) {
    const emptyComponent = props.ListEmptyComponent;
    if (emptyComponent) {
      children.push(
        typeof emptyComponent === "function"
          ? emptyComponent()
          : emptyComponent,
      );
    }
  } else {
    for (let index = start; index < end; index++) {
      const itemChildren = [
        withStateScope(
          `flat-list:${nodeId}:${JSON.stringify(keys[index])}`,
          () => props.renderItem({ item: data[index], index }),
        ),
      ];

      if (index < data.length - 1 && props.ItemSeparatorComponent) {
        itemChildren.push(
          props.ItemSeparatorComponent({
            leadingItem: data[index],
            trailingItem: data[index + 1],
          }),
        );
      }

      children.push({
        type: "View",
        key: keys[index],
        props: {
          direction: horizontal ? "row" : "column",
          listIndex: index,
          style: {},
        },
        children: itemChildren,
      });
    }
  }

  const listNode: VNode = {
    type: "View",
    props: {},
    children,
  };
  const serializedChildren = serializeChildren(listNode.children ?? [], nodeId);

  return {
    type: "FlatList",
    nodeId,
    props: {
      direction: horizontal ? "horizontal" : "vertical",
      scrollDirection:
        props.scrollDirection ?? (horizontal ? "horizontal" : "vertical"),
      scrollable,
      style: props.style ?? {},
      contentContainerStyle: contentStyle,
      totalCount: data.length,
      firstVisibleIndex: start,
      visibleItemCount: end - start,
      estimatedItemSize: itemSize,
      leadingExtent,
      trailingExtent,
      scrollOffset: viewport.offset,
    },
    events: {},
    children: serializedChildren,
  };
}

// Mengubah VNode menjadi JSON untuk dikirim ke C++.
function serialize(node: VNode, nodeId: string): any {
  if (node.type === "FlatList") {
    return serializeFlatList(node, nodeId);
  }

  return {
    type: node.type,
    nodeId,
    props: node.props,
    events: node.events || {},
    children: serializeChildren(node.children || [], nodeId),
  };
}

type LeafPatch = {
  nodeId: string;
  type: "Text" | "TextInput" | "Button";
  value: string;
};

let previousTree: any;

function sameValue(left: unknown, right: unknown): boolean {
  return JSON.stringify(left) === JSON.stringify(right);
}

function leafTextProperty(
  type: string,
): { property: string; patchType: LeafPatch["type"] } | undefined {
  if (type === "Text" || type === "Button") {
    return { property: "text", patchType: type };
  }
  if (type === "TextInput") {
    return { property: "value", patchType: type };
  }
  return undefined;
}

function collectLeafPatches(
  previous: any,
  next: any,
  patches: LeafPatch[],
  insideUnsupportedNode = false,
): boolean {
  if (
    previous.type !== next.type ||
    previous.nodeId !== next.nodeId ||
    previous.events === undefined ||
    !sameValue(previous.events, next.events) ||
    previous.children.length !== next.children.length
  ) {
    return false;
  }

  const unsupportedDescendants =
    insideUnsupportedNode ||
    ["Table", "Modal", "Video", "Image", "FloatingActionButton"].includes(
      next.type,
    );
  const previousProps = previous.props ?? {};
  const nextProps = next.props ?? {};
  const keys = new Set([
    ...Object.keys(previousProps),
    ...Object.keys(nextProps),
  ]);
  const changedKeys = [...keys].filter(
    (key) => !sameValue(previousProps[key], nextProps[key]),
  );

  if (changedKeys.length > 0) {
    const leaf = leafTextProperty(next.type);
    if (
      unsupportedDescendants ||
      leaf === undefined ||
      changedKeys.some((key) => key !== leaf.property)
    ) {
      return false;
    }

    patches.push({
      nodeId: next.nodeId,
      type: leaf.patchType,
      value: String(nextProps[leaf.property] ?? ""),
    });
  }

  for (let index = 0; index < previous.children.length; index++) {
    if (
      !collectLeafPatches(
        previous.children[index],
        next.children[index],
        patches,
        unsupportedDescendants,
      )
    ) {
      return false;
    }
  }
  return true;
}

function collectVirtualListPatches(
  previous: any,
  next: any,
  updates: any[],
): boolean {
  if (
    previous.type !== next.type ||
    previous.nodeId !== next.nodeId ||
    !sameValue(previous.events, next.events)
  ) {
    return false;
  }

  if (next.type === "FlatList") {
    const dynamicProps = new Set([
      "firstVisibleIndex",
      "scrollOffset",
      "totalCount",
      "visibleItemCount",
      "leadingExtent",
      "trailingExtent",
    ]);
    const previousProps = { ...previous.props };
    const nextProps = { ...next.props };
    for (const property of dynamicProps) {
      delete previousProps[property];
      delete nextProps[property];
    }
    if (!sameValue(previousProps, nextProps)) return false;
    if (!sameValue(previous, next)) updates.push(next);
    return true;
  }

  if (
    !sameValue(previous.props, next.props) ||
    previous.children.length !== next.children.length
  ) {
    return false;
  }

  for (let index = 0; index < previous.children.length; index++) {
    if (
      !collectVirtualListPatches(
        previous.children[index],
        next.children[index],
        updates,
      )
    ) {
      return false;
    }
  }
  return true;
}

// Mengirim UI ke native runtime.
export function render(root: VNode): void {
  activeVirtualListIds = new Set<string>();
  const result = serialize(root, "root");
  for (const nodeId of virtualListConfigs.keys()) {
    if (!activeVirtualListIds.has(nodeId)) {
      virtualListConfigs.delete(nodeId);
      virtualListViewports.delete(nodeId);
      virtualListItemSizes.delete(nodeId);
    }
  }
  let message: any = result;

  if (previousTree) {
    const patches: LeafPatch[] = [];
    if (collectLeafPatches(previousTree, result, patches)) {
      if (patches.length === 0) {
        previousTree = result;
        return;
      }
      message = { type: "patch", updates: patches };
    } else {
      const listUpdates: any[] = [];
      if (collectVirtualListPatches(previousTree, result, listUpdates)) {
        previousTree = result;
        if (listUpdates.length === 0) return;
        message = { type: "listPatch", updates: listUpdates };
      }
    }
  }

  previousTree = result;
  process.stdout.write("__JULIA_UI__" + JSON.stringify(message) + "\n");
}

// Menerima event dari native C++.
export function listenNativeEvents(): void {
  let buffer = "";

  process.stdin.setEncoding("utf8");

  process.stdin.on("data", (chunk: string) => {
    buffer += chunk;

    let index: number;

    while ((index = buffer.indexOf("\n")) !== -1) {
      const line = buffer.substring(0, index).trim();
      buffer = buffer.substring(index + 1);

      if (!line) continue;

      try {
        const message = JSON.parse(line);

        if (message.type === "click") {
          dispatchEvent(message.id);
          dispatchFloatingActionButtonPress(message.id);
        } else if (message.type === "change") {
          inputDispatchChange(message.id, message.value);
          checkboxDispatchChange(message.id, message.value);
        } else if (message.type === "modalClose") {
          dispatchModalClose(message.id);
        } else if (message.type === "tableSort") {
          dispatchTableSort(message.id, message.columnIndex, message.ascending);
        } else if (message.type === "tableResize") {
          dispatchTableResize(message.id, message.columnIndex, message.width);
        } else if (message.type === "windowEvent") {
          try {
            if (message.event === "resize") {
              dispatchWidgetEvent(message.id, message.width, message.height);
            } else {
              dispatchWidgetEvent(message.id);
            }
          } finally {
            if (message.event === "close") {
              process.stdout.write(
                '__JULIA_UI__{"type":"command","command":"windowCloseAck"}\n',
              );
            }
          }
        } else if (message.type === "listViewport") {
          updateListViewport(
            message.nodeId,
            Number(message.offset) || 0,
            Number(message.viewportSize) || 0,
            Array.isArray(message.items) ? message.items : [],
          );
        } else if (message.type === "storageResponse") {
          handleStorageResponse(message);
        } else if (message.type === "screensResponse") {
          Application.handleNativeResponse(message);
        } else if (message.type === "nativeResponse") {
          handleNativeResponse(message);
        }
      } catch (error) {
        console.error("Native event error:", error);
      }
    }
  });
}

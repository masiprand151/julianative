import { useState } from "../state";
import { StyleProps, TableColumn, TableProps, VNode } from "../types";

type SortState<T extends object> = {
  key: keyof T;
  ascending: boolean;
};

let sortEventCounter = 0;
let tableInstanceCounter = 0;
const columnWidthsByTable = new Map<number, (number | undefined)[]>();
const sortCallbacks = new Map<
  string,
  (columnIndex: number, ascending: boolean) => void
>();
const resizeCallbacks = new Map<
  string,
  (columnIndex: number, width: number) => void
>();

export function clearTableCallbacks(): void {
  sortCallbacks.clear();
  resizeCallbacks.clear();
}

export function dispatchTableSort(
  id: string,
  columnIndex: number,
  ascending: boolean,
): void {
  sortCallbacks.get(id)?.(columnIndex, ascending);
}

export function dispatchTableResize(
  id: string,
  columnIndex: number,
  width: number,
): void {
  resizeCallbacks.get(id)?.(columnIndex, width);
}

function compareValues(left: unknown, right: unknown): number {
  if (typeof left === "number" && typeof right === "number") {
    return left - right;
  }

  return String(left ?? "").localeCompare(String(right ?? ""), undefined, {
    numeric: true,
    sensitivity: "base",
  });
}

function columnWidth(width: TableColumn<object>["width"]): number | undefined {
  if (typeof width === "number") return width;
  if (typeof width !== "string") return undefined;
  const match = /^(\d+(?:\.\d+)?)(?:px)?$/.exec(width);
  return match ? Number(match[1]) : undefined;
}

export function Table<T extends object>(props: TableProps<T>): VNode {
  const [sort, setSort] = useState<SortState<T> | undefined>(undefined);
  const [tableId] = useState(() => ++tableInstanceCounter);
  const initialColumnWidths = props.columns.map((column) =>
    columnWidth(column.width),
  );
  const columnWidths = columnWidthsByTable.get(tableId) ?? initialColumnWidths;
  const sortEventId = `table_sort_${++sortEventCounter}`;
  const resizeEventId = `table_resize_${++sortEventCounter}`;
  sortCallbacks.set(sortEventId, (columnIndex, ascending) => {
    const column = props.columns[columnIndex];
    if (column) setSort({ key: column.key, ascending });
  });
  resizeCallbacks.set(resizeEventId, (columnIndex, width) => {
    const widths = [...columnWidths];
    widths[columnIndex] = width;
    columnWidthsByTable.set(tableId, widths);
  });

  const sortedIndices = props.data.map((_, index) => index);
  if (sort) {
    const column = props.columns.find(({ key }) => key === sort.key);
    if (column) {
      sortedIndices.sort((leftIndex, rightIndex) => {
        const left = props.data[leftIndex];
        const right = props.data[rightIndex];
        const comparison = column.compare
          ? column.compare(left, right)
          : compareValues(left[column.key], right[column.key]);

        if (comparison === 0) return leftIndex - rightIndex;
        return sort.ascending ? comparison : -comparison;
      });
    }
  }

  const children: VNode[] = [];
  sortedIndices.forEach((rowIndex) => {
    const item = props.data[rowIndex];
    props.columns.forEach((column) => {
      children.push(
        column.render
          ? column.render(item, rowIndex)
          : {
              type: "Text",
              props: { text: String(item[column.key] ?? "") },
            },
      );
    });
  });

  if (props.data.length === 0 && props.emptyComponent) {
    children.push(
      typeof props.emptyComponent === "function"
        ? props.emptyComponent()
        : props.emptyComponent,
    );
  }

  return {
    type: "Table",
    props: {
      columns: props.columns.map((column, index) => ({
        title: column.title,
        width: columnWidths[index],
        minWidth: columnWidth(column.minWidth),
        sortable: column.sortable !== false,
      })),
      rowCount: props.data.length,
      sortColumn:
        sort === undefined
          ? -1
          : props.columns.findIndex((column) => column.key === sort.key),
      sortAscending: sort?.ascending ?? true,
      style: props.style ?? {},
      headerStyle: props.headerStyle ?? {},
      headerTextStyle: props.headerTextStyle ?? {},
      rowStyle: props.rowStyle ?? {
        backgroundColor: "#ffffff",
        color: "#111111",
      },
      cellStyle: props.cellStyle ?? {},
      empty: props.data.length === 0 && props.emptyComponent !== undefined,
    },
    events: { sort: sortEventId, resize: resizeEventId },
    children,
  };
}

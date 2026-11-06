import { FlatListProps, VNode } from "../types";
import { View } from "./View";

export function FlatList<T>(props: FlatListProps<T>): VNode {
  const horizontal = props.horizontal ?? false;
  const direction = horizontal ? "row" : "column";
  const children: VNode[] = [];

  if (props.data.length === 0) {
    const emptyComponent = props.ListEmptyComponent;
    if (emptyComponent) {
      children.push(
        typeof emptyComponent === "function"
          ? emptyComponent()
          : emptyComponent,
      );
    }
  } else {
    props.data.forEach((item, index) => {
      if (index > 0 && props.ItemSeparatorComponent) {
        children.push(
          props.ItemSeparatorComponent({
            leadingItem: props.data[index - 1],
            trailingItem: item,
          }),
        );
      }

      children.push(props.renderItem({ item, index }));
    });
  }

  return View({
    direction,
    scrollable: props.scrollEnabled ?? true,
    scrollDirection:
      props.scrollDirection ?? (horizontal ? "horizontal" : "vertical"),
    style: props.style,
    children: [
      View({
        direction,
        style: props.contentContainerStyle,
        children,
      }),
    ],
  });
}

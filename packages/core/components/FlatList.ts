import { FlatListProps, VNode } from "../types";

export function FlatList<T>(props: FlatListProps<T>): VNode {
  return {
    type: "FlatList",
    props: { ...props },
  };
}

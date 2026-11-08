import { GridProps, VNode } from "../types";

export function Grid(props: GridProps): VNode {
  const columns = props.columns ?? 2;
  if (!Number.isInteger(columns) || columns < 1) {
    throw new RangeError("Grid columns must be a positive integer.");
  }

  const style = props.style ?? {};
  const gap = props.spacing ?? props.gap ?? style.gap ?? 0;

  return {
    type: "Grid",
    props: {
      columns,
      gap,
      rowGap: props.rowGap ?? style.rowGap ?? gap,
      columnGap: props.columnGap ?? style.columnGap ?? gap,
      padding: props.padding ?? style.padding ?? 0,
      style,
    },
    children: props.children ?? [],
  };
}

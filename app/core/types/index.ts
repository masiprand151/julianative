export type ComponentType =
  | "Widget"
  | "Column"
  | "Row"
  | "Text"
  | "Image"
  | "TextInput"
  | "Button"
  | "Video"
  | "View"
  | "Card"
  | "FloatingActionButton"
  | "Modal"
  | "Table";

export type EventHandler = (...args: any[]) => void;
export type EventMap = Record<string, string>;
type ScrollDirection = "vertical" | "horizontal" | "auto";

export interface VNode {
  type: ComponentType;
  props?: Record<string, any>;
  children?: VNode[];
  events?: EventMap;
}

export interface WidgetProps {
  title?: string;
  width?: number;
  height?: number;
  position?: { x: number; y: number };
  resizable?: boolean;
  fullscreen?: boolean;
  hideTitleBar?: boolean;
  alwaysOnTop?: boolean;
  scrollable?: boolean;
  onClose?: () => void;
  onResize?: (width: number, height: number) => void;
  onMaximize?: () => void;
  onMinimize?: () => void;
  style?: StyleProps;
  body: VNode[];
}

export interface StyleProps {
  width?: CSSLength;
  height?: CSSLength;
  minWidth?: CSSLength;
  minHeight?: CSSLength;
  maxWidth?: CSSLength;
  maxHeight?: CSSLength;
  margin?: CSSLength;
  padding?: CSSLength;
  gap?: CSSLength;
  backgroundColor?: string;
  background?: string;
  color?: string;
  border?: string;
  borderColor?: string;
  borderWidth?: CSSLength;
  borderRadius?: CSSLength;
  fontSize?: CSSLength;
  fontWeight?: "normal" | "bold";
  textAlign?: "left" | "center" | "right";
  cursor?: CursorType;
  flex?: number;
  fontFamily?: string;
  fontStyle?: "normal" | "italic";
  letterSpacing?: CSSLength;
  selectionColor?: string;
  selectionBackgroundColor?: string;
  ":hover"?: StyleProps;
  ":focus"?: StyleProps;
  ":active"?: StyleProps;
  ":pressed"?: StyleProps;
  ":disabled"?: StyleProps;
  [cssProperty: `${string}-${string}`]: string | number | undefined;
}

export type CSSLength =
  | number
  | `${number}px`
  | `${number}pt`
  | `${number}em`
  | `${number}ex`
  | `${number}%`;

export type CursorType =
  | "auto"
  | "default"
  | "none"
  | "context-menu"
  | "help"
  | "pointer"
  | "progress"
  | "wait"
  | "cell"
  | "crosshair"
  | "text"
  | "vertical-text"
  | "alias"
  | "copy"
  | "move"
  | "no-drop"
  | "not-allowed"
  | "grab"
  | "grabbing"
  | "all-scroll"
  | "col-resize"
  | "row-resize"
  | "n-resize"
  | "e-resize"
  | "s-resize"
  | "w-resize"
  | "ne-resize"
  | "nw-resize"
  | "se-resize"
  | "sw-resize"
  | "ew-resize"
  | "ns-resize"
  | "nesw-resize"
  | "nwse-resize"
  | "zoom-in"
  | "zoom-out";

export interface LayoutProps {
  spacing?: number;
  padding?: number;
  style?: ViewStyle;
  children?: VNode[];
}

export interface ViewStyle extends StyleProps {
  flexDirection?: "row" | "column";
  justifyContent?:
    | "flex-start"
    | "center"
    | "flex-end"
    | "space-between"
    | "space-around";
  alignItems?: "flex-start" | "center" | "flex-end" | "stretch";
}

export interface TextProps {
  fontSize?: number;
  fontWeight?: "normal" | "bold";
  foregroundColor?: string;
  style?: StyleProps;
}

export interface ButtonProps {
  text: string;
  onClick?: () => void;
  style?: StyleProps;
}

export interface ImageProps {
  source: string;
  alt?: string;
  resizeMode?: "contain" | "cover" | "fill";
  style?: StyleProps;
}

export interface FloatingActionButtonProps {
  icon?: string;
  imageSource?: string;
  accessibilityLabel?: string;
  placement?: "top-left" | "top-right" | "bottom-left" | "bottom-right";
  margin?: number;
  onPress?: () => void;
  style?: StyleProps;
}

export interface VideoProps {
  source: string;
  autoplay?: boolean;
  loop?: boolean;
  muted?: boolean;
  style?: StyleProps;
}

export interface ModalProps {
  visible?: boolean;
  title?: string;
  width?: number;
  height?: number;
  onClose?: () => void;
  style?: StyleProps;
  children?: VNode[];
}

export type TextInputType = "text" | "password" | "number" | "email";

export interface TextInputProps {
  type?: TextInputType;
  value?: string | number;
  placeholder?: string;
  autoFocus?: boolean;
  onChange?: (value: string) => void;
  style?: StyleProps;
}

export interface ViewProps extends LayoutProps {
  style?: ViewStyle;
  direction?: "row" | "column";
  scrollable?: boolean;
  scrollDirection?: ScrollDirection;
  justifyContent?:
    | "flex-start"
    | "center"
    | "flex-end"
    | "space-between"
    | "space-around";
  alignItems?: "flex-start" | "center" | "flex-end" | "stretch";
  flex?: number;
}

export interface FlatListRenderItemInfo<T> {
  item: T;
  index: number;
}

export interface FlatListSeparatorInfo<T> {
  leadingItem: T;
  trailingItem: T;
}

export interface FlatListProps<T> {
  data: readonly T[];
  renderItem: (info: FlatListRenderItemInfo<T>) => VNode;
  horizontal?: boolean;
  scrollDirection?: ScrollDirection;
  scrollEnabled?: boolean;
  ItemSeparatorComponent?: (
    info: FlatListSeparatorInfo<T>,
  ) => VNode;
  ListEmptyComponent?: VNode | (() => VNode);
  style?: StyleProps;
  contentContainerStyle?: StyleProps;
}

export interface TableColumn<T extends object> {
  key: keyof T;
  title: string;
  width?: CSSLength;
  minWidth?: CSSLength;
  sortable?: boolean;
  render?: (item: T, index: number) => VNode;
  compare?: (left: T, right: T) => number;
}

export interface TableProps<T extends object> {
  data: readonly T[];
  columns: readonly TableColumn<T>[];
  style?: StyleProps;
  headerStyle?: StyleProps;
  headerTextStyle?: StyleProps;
  rowStyle?: StyleProps;
  cellStyle?: StyleProps;
  emptyComponent?: VNode | (() => VNode);
}

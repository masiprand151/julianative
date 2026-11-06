export type ComponentType =
  | "Widget"
  | "Column"
  | "Row"
  | "Text"
  | "TextInput"
  | "Button"
  | "View";

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

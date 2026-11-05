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
  style?: StyleProps;
  body: VNode[];
}

export interface StyleProps {
  width?: number;
  height?: number;
  minWidth?: number;
  minHeight?: number;
  maxWidth?: number;
  maxHeight?: number;
  margin?: number;
  padding?: number;
  gap?: number;
  backgroundColor?: string;
  color?: string;
  borderColor?: string;
  borderWidth?: number;
  borderRadius?: number;
  fontSize?: number;
  fontWeight?: "normal" | "bold";
  textAlign?: "left" | "center" | "right";
  flex?: number;
}

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

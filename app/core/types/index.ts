export type ComponentType = "Widget" | "Column" | "Row" | "Text" | "Button";

export type EventHandler = (...args: any[]) => void;
export type EventMap = Record<string, string>;

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
  body: VNode[];
}

export interface LayoutProps {
  spacing?: number;
  padding?: number;
  children?: VNode[];
}

export interface TextProps {
  fontSize?: number;
  fontWeight?: "normal" | "bold";
  foregroundColor?: string;
}

export interface ButtonProps {
  text: string;
  onClick?: () => void;
}

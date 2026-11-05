import { StyleProps } from "./types";

export type StyleSheetDefinition = Record<string, StyleProps>;

export const StyleSheet = {
  create<T extends StyleSheetDefinition>(styles: T): T {
    return styles;
  },
};

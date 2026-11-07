import { StyleSheet } from "@julia/core/components";

export const styles = StyleSheet.create({
  primaryButton: {
    padding: "10px",
    backgroundColor: "#147d72",
    color: "#ffffff",
    borderRadius: "6px",
    cursor: "pointer",
    ":hover": {
      backgroundColor: "#282b2a",
    },
    ":pressed": {
      backgroundColor: "#0a4f48",
    },
  },
  textInput: {
    padding: "8px",
    backgroundColor: "#ffffff",
    border: "1px solid #9bbab0",
    borderRadius: "6px",
    ":hover": {
      borderColor: "#147d72",
    },
    ":focus": {
      borderColor: "#0f655c",
    },
  },
});

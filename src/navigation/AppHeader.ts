import { StackHeaderProps } from "../../app/core/navigation";
import { RootStackParamList } from "./types";
import { Button, Row, Text } from "@julia/core/components";

export function AppHeader({
  navigation,
  title,
  canGoBack,
}: StackHeaderProps<RootStackParamList>) {
  return Row({
    style: {
      gap: 12,
      alignItems: "center",
      padding: 8,
      backgroundColor: "#193b35",
      borderRadius: 6,
    },
    children: [
      ...(canGoBack
        ? [
            Button({
              text: "‹",
              onClick: navigation.goBack,
              style: {
                padding: "8px",
                color: "#ffffff",
                backgroundColor: "#347665",
                borderRadius: "6px",
                cursor: "pointer",
              },
            }),
          ]
        : []),
      Text(title, {
        style: {
          color: "#ffffff",
          fontSize: 18,
          fontWeight: "bold",
        },
      }),
    ],
  });
}

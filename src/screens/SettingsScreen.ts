import { Card, Text, Button } from "@julia/core/components";
import { StackScreenProps } from "../../app/core/navigation";
import { RootStackParamList } from "../navigation/types";
import { styles } from "../styles";

type Props = StackScreenProps<RootStackParamList, "Settings">;

export function SettingsScreen({ navigation, route }: Props) {
  return Card({
    title: "Pengaturan",
    children: [
      Text(`Bagian: ${route.params.section}`),
      Text("Data dikirim dengan navigation.navigate(route, params)."),
      Button({
        text: "Kembali ke Beranda",
        onClick: () => navigation.navigate("Home"),
        style: styles.primaryButton,
      }),
    ],
  });
}

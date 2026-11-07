import { Card, Text, Button } from "@julia/core/components";
import { StackScreenProps } from "../../app/core/navigation";
import { RootStackParamList } from "../navigation/types";
import { styles } from "../styles";

type Props = StackScreenProps<RootStackParamList, "Profile">;

export function ProfileScreen({ navigation, route }: Props) {
  return Card({
    title: "Profil",
    children: [
      Text(`Nama: ${route.params.name}`),
      Text(`Email: ${route.params.email || "Belum diisi"}`),
      Button({
        text: "Buka pengaturan",
        onClick: () =>
          navigation.navigate("Settings", { section: "Akun" }),
        style: styles.primaryButton,
      }),
    ],
  });
}

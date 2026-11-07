import { Card, Text, Button } from "@julia/core/components";
import { StackScreenProps } from "../../app/core/navigation";
import { RootStackParamList } from "../navigation/types";
import { styles } from "../styles";

type Props = StackScreenProps<RootStackParamList, "Details">;

export function DetailsScreen({ navigation, route }: Props) {
  return Card({
    title: "Detail",
    children: [
      Text(`Item ${route.params.itemId} dibuka dari ${route.params.from}`),
      Button({
        text: "Buka Profil",
        onClick: () =>
          navigation.navigate("Profile", {
            name: "Julian",
            email: route.params.email,
          }),
        style: styles.primaryButton,
      }),
      Button({
        text: "Buka Pengaturan",
        onClick: () =>
          navigation.navigate("Settings", { section: "Dari detail" }),
        style: styles.primaryButton,
      }),
    ],
  });
}

import { Application, createStackNavigator, useState } from "@julia/core";
import { Widget } from "@julia/core/components";
import { DetailsScreen } from "./screens/DetailsScreen";
import { HomeScreen } from "./screens/HomeScreen";
import { ProfileScreen } from "./screens/ProfileScreen";
import { SettingsScreen } from "./screens/SettingsScreen";
import { AppHeader } from "./navigation/AppHeader";
import { RootStackParamList } from "./navigation/types";

const Stack = createStackNavigator<RootStackParamList>();

function Main() {
  const [count, setCount] = useState(0);
  const [email, setEmail] = useState("");
  const [modalVisible, setModalVisible] = useState(false);

  return Widget({
    title: "Style Test",
    width: 1024,
    height: 768,
    position: { x: 0, y: 0 },
    fullscreen: false,
    resizable: true,
    hideTitleBar: false,
    alwaysOnTop: false,
    onClose: () => console.log("Window closed"),
    onResize: (width, height) =>
      console.log(`Window resized: ${width}x${height}`),
    onMaximize: () => console.log("Window maximized"),
    onMinimize: () => console.log("Window minimized"),
    style: {
      padding: 18,
      gap: 12,
      backgroundColor: "#f5f7f6",
    },
    body: [
      Stack.Navigator({
        initialRouteName: "Home",
        screenOptions: {
          headerComponent: AppHeader,
          headerStyle: {
            padding: 6,
            backgroundColor: "#dce9e5",
            borderRadius: 10,
          },
        },
        screens: {
          Home: {
            options: { title: "Beranda" },
            component: ({ navigation }) =>
              HomeScreen({
                navigation,
                count,
                setCount,
                email,
                setEmail,
                modalVisible,
                setModalVisible,
              }),
          },
          Profile: {
            options: { title: "Profil" },
            component: (props) => ProfileScreen(props),
          },
          Settings: {
            options: { title: "Pengaturan" },
            component: (props) => SettingsScreen(props),
          },
          Details: {
            options: { title: "Detail" },
            component: (props) => DetailsScreen(props),
          },
        },
      }),
    ],
  });
}

Application.run(Main);

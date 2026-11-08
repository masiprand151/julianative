import {
  Application,
  createStackNavigator,
  LocalStorage,
  useEffect,
  useState,
} from "@julia/core";
import { Widget } from "@julia/core/components";
import { DetailsScreen } from "./screens/DetailsScreen";
import { HomeScreen } from "./screens/HomeScreen";
import { ProfileScreen } from "./screens/ProfileScreen";
import { SettingsScreen } from "./screens/SettingsScreen";
import { AppHeader } from "./navigation/AppHeader";
import { RootStackParamList } from "./navigation/types";

const Stack = createStackNavigator<RootStackParamList>();

const windowStyle = {
  padding: 18,
  gap: 12,
  backgroundColor: "#f5f7f6",
};

const headerStyle = {
  margin: 6,
  padding: 6,
  backgroundColor: "#dce9e5",
  borderRadius: 10,
};

function Main() {
  const [count, setCount] = useState(0);
  const [countLoaded, setCountLoaded] = useState(false);
  const [email, setEmail] = useState("");
  const [modalVisible, setModalVisible] = useState(false);

  useEffect(() => {
    let active = true;
    LocalStorage.getItem("home-count")
      .then((savedCount) => {
        if (!active) return;
        if (typeof savedCount === "number" && Number.isFinite(savedCount)) {
          setCount(savedCount);
        }
        setCountLoaded(true);
      })
      .catch((error) => {
        console.error("Could not load saved count:", error);
        if (active) setCountLoaded(true);
      });

    return () => {
      active = false;
    };
  }, []);

  useEffect(() => {
    if (!countLoaded) return;

    LocalStorage.setItem("home-count", count).catch((error) => {
      console.error("Could not save count:", error);
    });
  }, [count, countLoaded]);

  return Widget({
    title: "Style Test",
    width: 1024,
    height: 768,
    onClose: () => console.log("Window closed"),
    onResize: (width, height) =>
      console.log(`Window resized: ${width}x${height}`),
    onMaximize: () => console.log("Window maximized"),
    onMinimize: () => console.log("Window minimized"),
    style: windowStyle,
    body: [
      Stack.Navigator({
        initialRouteName: "Home",
        screenOptions: {
          headerComponent: AppHeader,
          headerStyle,
        },
        screens: {
          Home: {
            options: { title: "Beranda" },
            component: ({ navigation }) =>
              HomeScreen({
                navigation,
                state: {
                  count,
                  setCount,
                  email,
                  setEmail,
                  modalVisible,
                  setModalVisible,
                },
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

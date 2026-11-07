import { useState } from "./state";
import { Button } from "./components/Button";
import { Column } from "./components/Column";
import { Row } from "./components/Row";
import { Text } from "./components/Text";
import { StyleProps, VNode } from "./types";

export type StackParamList = Record<string, object | undefined>;

export interface StackRoute<
  ParamList extends StackParamList,
  Name extends keyof ParamList = keyof ParamList,
> {
  key: string;
  name: Name;
  params?: ParamList[Name];
}

export interface StackNavigation<ParamList extends StackParamList> {
  navigate<Name extends keyof ParamList>(
    name: Name,
    params?: ParamList[Name],
  ): void;
  push<Name extends keyof ParamList>(
    name: Name,
    params?: ParamList[Name],
  ): void;
  replace<Name extends keyof ParamList>(
    name: Name,
    params?: ParamList[Name],
  ): void;
  goBack(): void;
  popToTop(): void;
  canGoBack(): boolean;
}

export interface StackScreenProps<
  ParamList extends StackParamList,
  Name extends keyof ParamList,
> {
  navigation: StackNavigation<ParamList>;
  route: StackRoute<ParamList, Name>;
}

export interface StackScreenOptions {
  title?: string;
  headerShown?: boolean;
}

export interface StackNavigatorProps<
  ParamList extends StackParamList,
  Name extends keyof ParamList,
> {
  initialRouteName: Name;
  screens: {
    [RouteName in keyof ParamList]: {
      component: (
        props: StackScreenProps<ParamList, RouteName>,
      ) => VNode;
      options?: StackScreenOptions;
    };
  };
  screenOptions?: StackScreenOptions;
  headerStyle?: StyleProps;
}

let routeKey = 0;

function createRoute<ParamList extends StackParamList, Name extends keyof ParamList>(
  name: Name,
  params?: ParamList[Name],
): StackRoute<ParamList, Name> {
  return { key: `stack_route_${++routeKey}`, name, params };
}

export function createStackNavigator<ParamList extends StackParamList>() {
  function Navigator<Name extends keyof ParamList>(
    props: StackNavigatorProps<ParamList, Name>,
  ): VNode {
    const [routes, setRoutes] = useState<StackRoute<ParamList>[]>(() => [
      createRoute(props.initialRouteName),
    ]);
    const activeRoute = routes[routes.length - 1];

    const navigation: StackNavigation<ParamList> = {
      navigate(name, params) {
        setRoutes((current) => {
          const existingIndex = current.findIndex(
            (route) => route.name === name,
          );
          if (existingIndex < 0) {
            return [...current, createRoute(name, params)];
          }

          const route = current[existingIndex];
          const activeRoute =
            params === undefined ? route : { ...route, params };
          if (
            existingIndex === current.length - 1 &&
            activeRoute === route
          ) {
            return current;
          }
          return [...current.slice(0, existingIndex), activeRoute];
        });
      },
      push(name, params) {
        setRoutes((current) => [...current, createRoute(name, params)]);
      },
      replace(name, params) {
        setRoutes((current) => [
          ...current.slice(0, -1),
          createRoute(name, params),
        ]);
      },
      goBack() {
        setRoutes((current) =>
          current.length > 1 ? current.slice(0, -1) : current,
        );
      },
      popToTop() {
        setRoutes((current) => current.slice(0, 1));
      },
      canGoBack() {
        return routes.length > 1;
      },
    };

    const screen = props.screens[activeRoute.name];
    const options = { ...props.screenOptions, ...screen.options };
    const children: VNode[] = [];

    if (options.headerShown !== false) {
      children.push(
        Row({
          style: {
            gap: 8,
            alignItems: "center",
            padding: 10,
            backgroundColor: "#ffffff",
            borderRadius: 6,
            ...props.headerStyle,
          },
          children: [
            ...(navigation.canGoBack()
              ? [
                  Button({
                    text: "‹",
                    onClick: navigation.goBack,
                    style: { cursor: "pointer" },
                  }),
                ]
              : []),
            Text(options.title ?? String(activeRoute.name), {
              style: { fontWeight: "bold" },
            }),
          ],
        }),
      );
    }

    children.push(
      screen.component({
        navigation,
        route: activeRoute as StackRoute<ParamList, typeof activeRoute.name>,
      }),
    );

    return Column({
      style: { gap: 12, flex: 1 },
      children,
    });
  }

  return { Navigator };
}

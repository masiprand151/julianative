export type RootStackParamList = {
  Home: undefined;
  Profile: { name: string; email: string };
  Settings: { section: string };
  Details: { from: string; itemId: number; email: string };
};

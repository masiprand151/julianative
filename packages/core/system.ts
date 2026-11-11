import { requestNative } from "./NativeBridge";

export interface CPUInfo {
  model: string;
  cores: number;
  architecture: string;
}

export interface MemoryInfo {
  total: number;
  free: number;
  used: number;
}

export interface SystemInfo {
  platform: string;
  osVersion: string;
  hostname: string;
  architecture: string;
  locale: string;
  timeZone: string;
}

export class System {
  static getPlatform(): Promise<string> {
    return requestNative("system.getPlatform");
  }

  static getOSVersion(): Promise<string> {
    return requestNative("system.getOSVersion");
  }

  static getCPUInfo(): Promise<CPUInfo> {
    return requestNative("system.getCPUInfo");
  }

  static getMemoryInfo(): Promise<MemoryInfo> {
    return requestNative("system.getMemoryInfo");
  }

  static getHostname(): Promise<string> {
    return requestNative("system.getHostname");
  }

  static getLocale(): Promise<string> {
    return requestNative("system.getLocale");
  }

  static getTimeZone(): Promise<string> {
    return requestNative("system.getTimeZone");
  }

  static getSystemInfo(): Promise<SystemInfo> {
    return requestNative("system.getSystemInfo");
  }
}

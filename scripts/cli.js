#!/usr/bin/env node

const { spawnSync } = require("child_process");
const path = require("path");
const fs = require("fs");
const esbuild = require("esbuild");

const ROOT = path.resolve(__dirname, "..");

const CONFIG = {
  buildDir: path.join(ROOT, "packages/build"),
  distDir: path.join(ROOT, "packages/build/js"),

  entry: path.join(ROOT, "src/main.ts"),
  output: path.join(ROOT, "packages/build/js/julia.cjs"),

  appExe: path.join(ROOT, "packages/build/JuliaNative.exe"),

  gcc: "C:/msys64/ucrt64/bin/gcc.exe",
  gpp: "C:/msys64/ucrt64/bin/g++.exe",

  tests: [
    "tests/components.test.cjs",
    "tests/navigation.test.cjs",
    "tests/renderer.test.cjs",
    "tests/localStorage.test.cjs",
    "tests/state.test.cjs",
  ],
};

// Execute command.
function run(command, args = []) {
  console.log(`\n> ${command} ${args.join(" ")}`);

  const result = spawnSync(command, args, {
    cwd: ROOT,
    stdio: "inherit",
    shell: false,
    windowsHide: false,
  });

  if (result.error) {
    throw result.error;
  }

  if (result.signal) {
    throw new Error(`Command dihentikan: ${command} (${result.signal})`);
  }

  if (result.status !== 0) {
    throw new Error(`Command gagal: ${command} (exit code: ${result.status})`);
  }
}

// Execute JavaScript file.
function node(file, args = []) {
  run(process.execPath, [path.resolve(ROOT, file), ...args]);
}

// Build TypeScript with esbuild.
async function buildTS(options = {}) {
  if (!fs.existsSync(CONFIG.entry)) {
    throw new Error(`Entry tidak ditemukan: ${CONFIG.entry}`);
  }

  fs.mkdirSync(CONFIG.distDir, {
    recursive: true,
  });

  const release = options.release === true;

  console.log("\nBuilding TypeScript with esbuild...");

  await esbuild.build({
    entryPoints: [CONFIG.entry],
    outfile: CONFIG.output,

    bundle: true,
    platform: "node",
    format: "cjs",

    target: "node20",

    minify: release,
    sourcemap: !release,

    treeShaking: true,

    tsconfig: path.join(ROOT, "tsconfig.json"),

    logLevel: "info",
  });

  console.log("\nTypeScript build successful!");
}

// Check TypeScript types.
function checkTS() {
  run(process.execPath, [require.resolve("typescript/bin/tsc"), "--noEmit"]);
}

// Configure CMake.
function configureApp() {
  run("cmake", [
    "-S",
    ".",
    "-B",
    "packages/build",
    "-G",
    "Ninja",
    "-DCMAKE_BUILD_TYPE=Debug",
    `-DCMAKE_C_COMPILER=${CONFIG.gcc}`,
    `-DCMAKE_CXX_COMPILER=${CONFIG.gpp}`,
  ]);
}

// Build application.
async function buildApp(options = {}) {
  await buildTS(options);

  configureApp();

  run("cmake", ["--build", "packages/build"]);
}

// Run application.
async function startApp(options = {}) {
  await buildApp(options);

  if (!fs.existsSync(CONFIG.appExe)) {
    throw new Error("JuliaNative.exe tidak ditemukan.");
  }

  console.log("\nStarting JuliaNative...\n");

  run(CONFIG.appExe);
}

// Run tests.
async function test(options = {}) {
  await buildTS(options);

  run(process.execPath, [
    "--test",
    ...CONFIG.tests.map((file) => path.resolve(ROOT, file)),
  ]);
}

// Development.
async function dev(options = {}) {
  await buildApp(options);

  node("scripts/dev.cjs");
}

// Reload.
async function reload(options = {}) {
  await buildTS(options);

  node("scripts/signal-reload.cjs");
}

// Portable.
async function preparePortable() {
  node("scripts/prepare-portable.cjs");
}

async function portable(options = {}) {
  await buildApp(options);

  await preparePortable();
}

// Clean.
function clean() {
  if (fs.existsSync(CONFIG.buildDir)) {
    fs.rmSync(CONFIG.buildDir, {
      recursive: true,
      force: true,
    });
  }

  console.log("\nBuild berhasil dibersihkan.");
}

// Help.
function help() {
  console.log(`
JuliaNative CLI

Commands:
  build:ts          Build TypeScript
  check:ts          Check TypeScript
  test              Run tests
  configure:app     Configure CMake
  build:app         Build application
  app               Build and run
  dev               Development
  dev:reload        Reload
  prepare:portable  Prepare portable
  portable          Build portable
  clean             Clean build

Shortcuts:
  build             Build application
  check             Check TypeScript
  reload            Reload
  help              Show help

Options:
  --release         Minified production build
`);
}

// Commands.
const commands = {
  "build:ts": buildTS,
  "check:ts": checkTS,
  test,
  "configure:app": configureApp,
  "build:app": buildApp,
  app: startApp,
  dev,
  "dev:reload": reload,
  "prepare:portable": preparePortable,
  portable,
  clean,

  build: buildApp,
  check: checkTS,
  reload,
  help,
};

// Main.
async function main() {
  const args = process.argv.slice(2);
  const command = args[0] || "help";

  try {
    const action = commands[command];

    if (!action) {
      throw new Error(`Command tidak dikenal: ${command}`);
    }

    await action({
      release: args.includes("--release"),
    });
  } catch (error) {
    console.error("\n[JuliaNative CLI Error]");
    console.error(error.message);
    process.exitCode = 1;
  }
}

main();

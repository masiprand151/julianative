const { execFileSync } = require("node:child_process");
const fs = require("node:fs");
const path = require("node:path");

const root = path.resolve(__dirname, "..");
const packageJson = JSON.parse(
  fs.readFileSync(path.join(root, "package.json"), "utf8"),
);
const appName = packageJson.name;
if (typeof appName !== "string" || !/^[a-zA-Z0-9._-]+$/.test(appName)) {
  throw new Error(
    "package.json name must contain only letters, numbers, dots, underscores, and hyphens for the portable executable.",
  );
}
const portableExecutableName = `${appName}.exe`;

const portable = path.join(root, "portable");
const runtime = path.join(portable, "runtime");

const nodeSource = path.join(root, "packages", "node", "node.exe");
const buildDir = path.join(root, "packages", "build");
const executable = path.join(buildDir, "JuliaNative.exe");
const cmakeCachePath = path.join(buildDir, "CMakeCache.txt");

const sourceApp = path.join(buildDir, "packages");
const sourceSrc = path.join(buildDir, "src");
const sourceAssets = path.join(root, "assets");

function requirePath(file) {
  if (!fs.existsSync(file)) {
    throw new Error(`File atau folder tidak ditemukan: ${file}`);
  }
}

function findDllFiles(directory) {
  const files = [];
  for (const entry of fs.readdirSync(directory, { withFileTypes: true })) {
    const file = path.join(directory, entry.name);
    if (entry.isDirectory()) {
      files.push(...findDllFiles(file));
    } else if (entry.isFile() && /\.dll$/i.test(entry.name)) {
      files.push(file);
    }
  }
  return files;
}

requirePath(nodeSource);
requirePath(executable);
requirePath(cmakeCachePath);
requirePath(sourceApp);
requirePath(sourceSrc);

const cmakeCache = fs.readFileSync(cmakeCachePath, "utf8");
const qtDirMatch = cmakeCache.match(/^Qt6_DIR:[^=]*=(.+)$/m);
if (!qtDirMatch) {
  throw new Error("Qt6_DIR was not found in packages/build/CMakeCache.txt.");
}

const buildTypeMatch = cmakeCache.match(/^CMAKE_BUILD_TYPE:[^=]*=(.+)$/m);
if (!buildTypeMatch) {
  throw new Error(
    "CMAKE_BUILD_TYPE was not found in packages/build/CMakeCache.txt.",
  );
}

const qtRoot = path.resolve(qtDirMatch[1].trim(), "..", "..", "..");
const qtDeployTool = path.join(qtRoot, "bin", "windeployqt.exe");
requirePath(qtDeployTool);

fs.rmSync(portable, {
  recursive: true,
  force: true,
});

fs.mkdirSync(path.join(runtime, "node"), {
  recursive: true,
});

fs.mkdirSync(path.join(runtime, "packages"), {
  recursive: true,
});

// Salin executable Qt.
const portableExecutable = path.join(portable, portableExecutableName);
fs.copyFileSync(executable, portableExecutable);

execFileSync(
  qtDeployTool,
  [
    buildTypeMatch[1].trim().toLowerCase() === "debug"
      ? "--debug"
      : "--release",
    "--compiler-runtime",
    "--no-translations",
    "--no-quick-import",
    "--dir",
    portable,
    "--libdir",
    portable,
    "--plugindir",
    path.join(portable, "plugins"),
    portableExecutable,
  ],
  { stdio: "inherit" },
);

requirePath(path.join(portable, "plugins", "platforms", "qwindows.dll"));
fs.writeFileSync(
  path.join(portable, "qt.conf"),
  "[Paths]\nPrefix = .\nPlugins = plugins\n",
);

const msysRoot = path.resolve(qtRoot, "..");
const lddTool = path.join(msysRoot, "usr", "bin", "ldd.exe");
requirePath(lddTool);

const dependencySearchPath = [
  path.join(qtRoot, "bin"),
  path.join(msysRoot, "usr", "bin"),
  process.env.PATH ?? "",
].join(path.delimiter);
const packagedBinaries = [portableExecutable, ...findDllFiles(portable)];
const runtimeDependencies = new Map();

for (const binary of packagedBinaries) {
  const output = execFileSync(lddTool, [binary], {
    encoding: "utf8",
    env: { ...process.env, PATH: dependencySearchPath },
  });
  for (const line of output.split(/\r?\n/)) {
    if (/=>\s+not found/i.test(line)) {
      throw new Error(
        `Unresolved runtime dependency for ${binary}: ${line.trim()}`,
      );
    }

    const dependency = line.match(/=>\s+([^\s(]+)\s+\(/)?.[1];
    const msysPath = dependency?.match(/^\/(ucrt64|mingw64|usr)\/(.+)$/i);
    if (!msysPath) continue;

    const source = path.join(msysRoot, msysPath[1], msysPath[2]);
    const destination = path.join(portable, path.basename(source));
    requirePath(source);
    if (!fs.existsSync(destination)) {
      runtimeDependencies.set(destination, source);
    }
  }
}

for (const [destination, source] of runtimeDependencies) {
  fs.copyFileSync(source, destination);
}

console.log(
  `Copied ${runtimeDependencies.size} MSYS2 runtime DLL dependencies.`,
);

// Salin Node.js portable.
fs.copyFileSync(nodeSource, path.join(runtime, "node", "node.exe"));

// Salin hasil kompilasi TypeScript.
fs.cpSync(sourceSrc, path.join(runtime, "packages", "src"), {
  recursive: true,
});

fs.cpSync(sourceApp, path.join(runtime, "packages", "packages"), {
  recursive: true,
});

if (fs.existsSync(sourceAssets) && fs.statSync(sourceAssets).isDirectory()) {
  fs.cpSync(sourceAssets, path.join(runtime, "packages", "assets"), {
    recursive: true,
  });
}

console.log("");
console.log(`${appName} portable berhasil dibuat.`);
console.log(`Executable: ${portableExecutableName}`);
console.log(`Lokasi: ${portable}`);
console.log("");

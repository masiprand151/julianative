const fs = require("node:fs");
const path = require("node:path");

const root = path.resolve(__dirname, "..");

const portable = path.join(root, "portable");
const runtime = path.join(portable, "runtime");

const nodeSource = path.join(root, "app", "node", "node.exe");
const buildDir = path.join(root, "app", "build");
const executable = path.join(buildDir, "JuliaNative.exe");

const sourceApp = path.join(buildDir, "app");
const sourceSrc = path.join(buildDir, "src");

function requirePath(file) {
  if (!fs.existsSync(file)) {
    throw new Error(`File atau folder tidak ditemukan: ${file}`);
  }
}

requirePath(nodeSource);
requirePath(executable);
requirePath(sourceApp);
requirePath(sourceSrc);

fs.rmSync(portable, {
  recursive: true,
  force: true,
});

fs.mkdirSync(path.join(runtime, "node"), {
  recursive: true,
});

fs.mkdirSync(path.join(runtime, "app"), {
  recursive: true,
});

// Salin executable Qt.
fs.copyFileSync(executable, path.join(portable, "JuliaNative.exe"));

// Salin Node.js portable.
fs.copyFileSync(nodeSource, path.join(runtime, "node", "node.exe"));

// Salin hasil kompilasi TypeScript.
fs.cpSync(sourceSrc, path.join(runtime, "app", "src"), { recursive: true });

fs.cpSync(sourceApp, path.join(runtime, "app", "app"), { recursive: true });

console.log("");
console.log("JuliaNative portable berhasil dibuat.");
console.log(`Lokasi: ${portable}`);
console.log("");

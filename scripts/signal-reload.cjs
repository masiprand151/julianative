const fs = require("node:fs");
const path = require("node:path");

const root = path.resolve(__dirname, "..");

const signalPath = path.join(root, "packages", "build", ".dev-reload");

if (!fs.existsSync(signalPath)) {
  throw new Error("Development app is not running; reload signal is missing.");
}

fs.writeFileSync(signalPath, String(Date.now()), "utf8");

console.log("[JuliaNative] Reload signal sent.");

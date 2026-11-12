const { execFileSync } = require("node:child_process");
const path = require("node:path");

const root = path.resolve(__dirname, "../..");

try {
  console.log("\n[JuliaNative] Compiling TypeScript...\n");

  execFileSync(
    process.execPath,
    [
      path.join(root, "node_modules", "typescript", "bin", "tsc"),
      "--project",
      path.join(root, "tsconfig.json"),
    ],
    {
      cwd: root,
      stdio: "inherit",
    },
  );

  console.log("\n[JuliaNative] TypeScript build completed.\n");
} catch (error) {
  console.error("\n[JuliaNative] TypeScript build failed.\n");
  process.exitCode = 1;
}

import { IterateLSD, LSD, PathKind } from "./lib/Cxx/LSD.mts";

const tree = [];

const { stdout } = await LSD({ path: "." });
for (const result of stdout?.split("\n") ?? []) {
  tree.push(result);
}
console.log(tree);

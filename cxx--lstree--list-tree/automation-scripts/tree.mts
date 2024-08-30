import { LSD } from "./lib/Cxx/LSD.mts";

const tree = ["D ."];

for (let index = 0; index < tree.length; index++) {
  if (tree[index][0] === "D") {
    const root = tree[index].slice(2);
    // console.log(root);
    const { stdout } = await LSD({ path: root });
    const paths = (stdout ?? "")?.split("\n").sort();
    for (let index = 0; index < paths.length; index++) {
      if (paths[index][0] === "D") {
        tree.push(paths[index][0] + " " + root + "\\" + paths[index].slice(2));
      }
      if (paths[index][0] === "F") {
        tree.push(paths[index][0] + " " + root + "\\" + paths[index].slice(2));
      }
    }
  }
}
console.log(tree);

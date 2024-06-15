import node_fs from 'node:fs/promises';
import { IterateLSD, LSD, PathKind } from './lib/Cxx/LSD.mts';

IterateLSD(LSD({ filter: 'cxx--*' }), PathKind.Directory, ({ path: directory }) => {
  IterateLSD(LSD({ path: directory, filter: '*.exe' }), PathKind.File, ({ path: file }) => {
    node_fs
      .copyFile(`.\\${directory}\\${file}`, `.\\${file}`)
      .then(() => {
        console.log(`Copied: ${file}`);
      })
      .catch(() => {
        console.log(`Error: Copy failed: ${file}`);
      });
  });
});

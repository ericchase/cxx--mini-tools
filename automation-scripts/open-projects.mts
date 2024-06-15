import node_child_process from 'node:child_process';
import { IterateLSD, LSD, PathKind } from './lib/Cxx/LSD.mts';

IterateLSD(LSD({ filter: 'cxx--*' }), PathKind.Directory, ({ path: directory }) => {
  node_child_process.execFile('code', [directory]);
});

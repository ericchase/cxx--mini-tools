import * as child_process from 'node:child_process';
import { watch } from './lib.watch.mjs';

/**
 * @param {object} params
 * @param {string} params.command
 * @param {string[]=} params.args
 * @param {*=} params.options
 * @param {(changes:*)=>void=} stdout_cb
 * @param {(errors:*)=>void=} stderr_cb
 */
async function spawn({ command, args = [], options = {} }, stdout_cb = (_) => {}, stderr_cb = (_) => {}) {
  return new Promise((resolve, reject) => {
    const ps = child_process.spawn(command, args, options);
    ps.on('close', (code) => resolve(code));
    ps.on('error', (error) => reject(error));
    ps.stdout.on('data', stdout_cb);
    ps.stderr.on('data', stderr_cb);
  });
}

try {
  await spawn(
    { command: 'cmake', args: '--build .\\build --config Debug --target ALL_BUILD -j 8 --'.split(' ') }, //
    (chunk) => console.log(chunk.slice(0, -2).toString('utf8')),
    (chunk) => console.log(chunk.slice(0, -2).toString('utf8'))
  );
  console.log();
  console.log(
    'Exit Code:',
    await watch({
      path: './test', //
      debounce: 100,
      change_cb: (changes) => {
        for (const change of changes) {
          switch (change[0]) {
            case '0':
              console.log('Watching', change.slice(2));
              break;
            case '1':
              console.log('   Added', change.slice(2));
              break;
            case '2':
              console.log(' Removed', change.slice(2));
              break;
            case '3':
              console.log('Modified', change.slice(2));
              break;
            case '4':
              console.log(' Renamed', change.slice(2).split('\t').join(' -> '));
              break;
          }
        }
      },
      error_cb: (error) => console.log('Error:', error.slice(0, -2)),
    })
  );
} catch (err) {
  console.log(err);
}

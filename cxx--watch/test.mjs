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

function removeNewlineEnding(data) {
  if (data.endsWith('\r\n')) {
    return data.slice(0, -2);
  }
  if (data.endsWith('\n')) {
    return data.slice(0, -1);
  }
}

try {
  await spawn(
    { command: 'cmake', args: '--build .\\build --config Debug --target ALL_BUILD -j 8 --'.split(' ') }, //
    (chunk) => console.log(removeNewlineEnding(chunk.toString())),
    (chunk) => console.log(removeNewlineEnding(chunk.toString()))
  );
  console.log();
  console.log(
    'Exit Code:',
    await watch({
      path: './test', //
      debounce_interval: 1000,
      change_cb: (changes) => {
        for (const change of changes) {
          switch (change[0]) {
            case '0':
              console.log('Watching', change);
              break;
            case '1':
              console.log('   Added', change);
              break;
            case '2':
              console.log(' Removed', change);
              break;
            case '3':
              console.log('Modified', change);
              break;
            case '4':
              console.log(' Renamed', change.split('\t').join(' -> '));
              break;
          }
        }
      },
      error_cb: (error) => console.log('ERROR:', error),
    })
  );
} catch (err) {
  console.log(err);
}

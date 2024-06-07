import * as child_process from 'node:child_process';
import { watch } from './lib.watch.mjs';

async function execFile(file, args = [], options = {}) {
  return new Promise((resolve, reject) => {
    child_process.execFile(file, args, options, (error, stdout, stderr) => {
      if (error) reject(error);
      resolve({ error, stdout, stderr });
    });
  });
}

try {
  const build = await execFile('cmake', '--build .\\build --config Debug --target ALL_BUILD -j 8 --'.split(' '));
  if (build.stdout) console.log(build.stdout);
  if (build.stderr) console.log(build.stderr);
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

import node_child_process from 'node:child_process';
import { Watch } from './lib-watch.mjs';

/**
 * @param {string} program
 * @param {string[]} args
 * @param {import('node:fs').ObjectEncodingOptions & import('node:child_process').ExecFileOptions} options
 * @returns {Promise<{stdout:string,stderr:string}>}
 */
export function run(program, args, options = {}) {
  return new Promise((resolve, reject) => {
    node_child_process.execFile(program, args, (error, stdout, stderr) => {
      if (error) return reject(error);
      return resolve({ stdout, stderr });
    });
  });
}

/**
 * @param {()=>void} callback
 * @param {number} delay
 */
function debounce(callback, delay) {
  let timer = /**@type{NodeJS.Timeout | undefined}*/ (undefined);
  return function () {
    clearTimeout(timer);
    timer = setTimeout(() => {
      callback();
    }, delay);
  };
}

try {
  const build = await run('cmake', '--build .\\build --config Debug --target ALL_BUILD -j 8 --'.split(' '));
  if (build.stdout) console.log(build.stdout);
  if (build.stderr) console.log(build.stderr);
  console.log();
  console.log(
    'Exit Code:',
    await Watch({
      path: './test', //
      watch_path: '.\\build\\Debug\\watch.exe',
      debounce_interval: 250,
      change_cb: (changes) => {
        for (const change of changes) {
          switch (change[0]) {
            case 'S':
              console.log('watching >', change.slice(2));
              break;
            case 'C':
              console.log('   added >', change.slice(2));
              break;
            case 'D':
              console.log(' removed >', change.slice(2));
              break;
            case 'M':
              console.log('modified >', change.slice(2));
              break;
            case 'R':
              console.log(' renamed >', change.slice(2).split('\t').join(' -> '));
              break;
          }
        }
      },
      error_cb: (error) => console.error('ERROR:', error),
    })
  );
} catch (err) {
  console.error(err);
}

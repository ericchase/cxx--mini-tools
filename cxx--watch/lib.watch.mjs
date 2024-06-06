import * as child_process from 'node:child_process';

/**
 * @param {object} params
 * @param {string} params.path
 * @param {number=} params.debounce
 * @param {(changes:string[])=>void=} params.change_cb
 * @param {(error:string)=>void=} params.error_cb
 */
export function watch({ path, debounce = 0, change_cb = (_) => {}, error_cb = (_) => {} }) {
  return new Promise(async (resolve, reject) => {
    const p = child_process.spawn('watch.exe', [path]);
    p.on('close', (code) => {
      resolve(code);
    });
    p.on('error', (error) => {
      reject(error);
    });
    if (debounce > 0) {
      let changes = [];
      p.stdout.on('data', (chunk) => {
        changes.push(chunk.slice(0, -2).toString('utf8'));
      });
      setInterval(() => {
        if (changes.length > 0) {
          change_cb(changes);
          changes = [];
        }
      }, debounce).unref();
    } else {
      p.stdout.on('data', (chunk) => {
        change_cb([chunk.toString('utf8')]);
      });
    }
    p.stderr.on('data', (chunk) => {
      error_cb(chunk.toString('utf8'));
    });
  });
}

import * as child_process from 'node:child_process';

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
  const run = await execFile('.\\build\\Debug\\mkd.exe', ['1/😊/1', '2/😊/2']);
  if (run.stdout) console.log(run.stdout);
  if (run.stderr) console.log(run.stderr);
} catch (err) {
  console.log(err);
}

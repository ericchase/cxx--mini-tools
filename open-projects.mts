import node_child_process from 'node:child_process';
import node_fs from 'node:fs/promises';

function LSD(path: string, filter: string) {
  return new Promise<{ stdout: string; stderr: string }>((resolve, reject) => {
    node_child_process.execFile('lsd.exe', [path, filter], (error, stdout, stderr) => {
      if (error) return reject(error);
      return resolve({ stdout, stderr });
    });
  });
}

const { stdout, stderr } = await LSD('.', 'cxx--*');
if (stderr) {
  console.log('Error:', stderr);
}
if (stdout) {
  for (const line of stdout.split('\n')) {
    if (line[0] === 'D') {
      const directory = line.slice(2);
      node_child_process.execFile('code', [directory]);
    }
  }
}

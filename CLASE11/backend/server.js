const http = require('http');

const server = http.createServer((req, res) => {
 res.setHeader('Access-Control-Allow-Origin', '*');
 if (req.url === '/enviar') {
    console.log('Enviar recibido');
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('ok');
  } else {
    res.writeHead(404);
    res.end();
  }
});

server.listen(3000, () => {
  console.log('Servidor corriendo en puerto 3000');
});

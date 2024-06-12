const http = require('http');

const hostname = '0.0.0.0';
const port = 3000;

async function main(data) {
    try{
        console.log(data);

    } catch (error) {
        console.error("Encountered an error: ", error);
    }
}


const server = http.createServer((req, res) => {
    console.log("\nRecibiendo peticion...");
    console.log(new Date().toLocaleString());
    if (req.method === 'POST' && req.headers['content-type'] === 'application/json') {
        let body = '';
        req.on('data', chunk => {
            body += chunk.toString(); // convert Buffer to string
        });
        req.on('end', () => {
            const data = JSON.parse(body); // parse the JSON data
            res.end('JSON received and printed');
            main(data);
        });
    } else {
        res.statusCode = 200;
        res.setHeader('Content-Type', 'text/plain');
        res.end('Hello, World!');
        console.log("Hello, World!");
    }
});

server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`);
});

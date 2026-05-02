const http = require("http");
const fs = require("fs");
const path = require("path");
const { execFile } = require("child_process");

const PORT = 3000;
const ROOT = __dirname;
const BACKEND = path.join(ROOT, "backend.exe");

const contentTypes = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
};

function sendJson(res, statusCode, payload) {
  res.writeHead(statusCode, { "Content-Type": "application/json; charset=utf-8" });
  res.end(JSON.stringify(payload));
}

function readJson(req) {
  return new Promise((resolve, reject) => {
    let body = "";

    req.on("data", chunk => {
      body += chunk;
      if (body.length > 10000) {
        reject(new Error("Request body too large"));
        req.destroy();
      }
    });

    req.on("end", () => {
      try {
        resolve(body ? JSON.parse(body) : {});
      } catch (error) {
        reject(new Error("Invalid JSON"));
      }
    });

    req.on("error", reject);
  });
}

function runBackend(args) {
  return new Promise((resolve, reject) => {
    execFile(BACKEND, args, { cwd: ROOT }, (error, stdout, stderr) => {
      const output = (stdout || stderr || "").trim();

      if (error) {
        reject(new Error(output || error.message));
        return;
      }

      resolve(output);
    });
  });
}

function isValidRoll(roll) {
  return /^\d+$/.test(String(roll));
}

function serveStatic(req, res) {
  const urlPath = req.url === "/" ? "/index.html" : req.url.split("?")[0];
  const safePath = path.normalize(urlPath).replace(/^(\.\.[/\\])+/, "");
  const filePath = path.join(ROOT, safePath);

  if (!filePath.startsWith(ROOT)) {
    res.writeHead(403);
    res.end("Forbidden");
    return;
  }

  fs.readFile(filePath, (error, content) => {
    if (error) {
      res.writeHead(404);
      res.end("Not found");
      return;
    }

    const type = contentTypes[path.extname(filePath)] || "application/octet-stream";
    res.writeHead(200, { "Content-Type": type });
    res.end(content);
  });
}

async function handleApi(req, res) {
  try {
    if (req.method === "POST" && req.url === "/api/students") {
      const { roll, name } = await readJson(req);

      if (!isValidRoll(roll) || !name) {
        sendJson(res, 400, { ok: false, message: "Roll number and name are required." });
        return;
      }

      if (String(name).length > 49) {
        sendJson(res, 400, { ok: false, message: "Name must be 49 characters or less." });
        return;
      }

      const message = await runBackend(["add", String(roll), String(name)]);
      sendJson(res, 200, { ok: true, message });
      return;
    }

    if (req.method === "POST" && req.url === "/api/attendance") {
      const { roll, status } = await readJson(req);

      if (!isValidRoll(roll) || !status) {
        sendJson(res, 400, { ok: false, message: "Roll number and status are required." });
        return;
      }

      if (!["Present", "Absent"].includes(status)) {
        sendJson(res, 400, { ok: false, message: "Status must be Present or Absent." });
        return;
      }

      const message = await runBackend(["mark", String(roll), String(status)]);
      sendJson(res, 200, { ok: true, message });
      return;
    }

    if (req.method === "GET" && req.url === "/api/attendance") {
      const message = await runBackend(["display"]);
      sendJson(res, 200, { ok: true, message });
      return;
    }

    sendJson(res, 404, { ok: false, message: "API route not found." });
  } catch (error) {
    sendJson(res, 500, { ok: false, message: error.message });
  }
}

const server = http.createServer((req, res) => {
  if (req.url.startsWith("/api/")) {
    handleApi(req, res);
    return;
  }

  serveStatic(req, res);
});

server.listen(PORT, () => {
  console.log(`Attendance system running at http://localhost:${PORT}`);
});

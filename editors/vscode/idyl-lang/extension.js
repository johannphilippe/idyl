// extension.js — Idyl live-coding integration for VS Code
// Sends OSC UDP packets to a running `idyl --listen` instance.
//
// Commands:
//   idyl.eval          — hot-reload the construct at the cursor  (Ctrl+Enter)
//   idyl.startProcess  — start the process block at the cursor   (Ctrl+Alt+S)
//   idyl.stopProcess   — stop  the process block at the cursor   (Ctrl+Alt+Q)

'use strict';

const vscode = require('vscode');
const dgram  = require('dgram');

// ── OSC helpers ───────────────────────────────────────────────────────────────

function oscPad(buf) {
    const pad = (4 - (buf.length % 4)) % 4;
    return pad === 0 ? buf : Buffer.concat([buf, Buffer.alloc(pad)]);
}

function oscString(s) {
    return oscPad(Buffer.from(s + '\0', 'utf8'));
}

function buildOscMessage(address, payload) {
    return Buffer.concat([oscString(address), oscString(',s'), oscString(payload)]);
}

function sendOsc(address, payload) {
    const cfg  = vscode.workspace.getConfiguration('idyl');
    const host = cfg.get('oscHost', '127.0.0.1');
    const port = cfg.get('oscPort', 9000);

    const msg  = buildOscMessage(address, payload);
    const sock = dgram.createSocket('udp4');
    sock.send(msg, port, host, (err) => {
        sock.close();
        if (err) {
            vscode.window.showWarningMessage(`[idyl] OSC send failed: ${err.message}`);
        }
    });
}

// ── Block detection (mirrors Vim's BlockStart / BlockEnd) ────────────────────

function blockStart(document, lineNum) {
    for (let l = lineNum; l >= 0; l--) {
        const text = document.lineAt(l).text;
        if (/^\S/.test(text) && !/^}\s*$/.test(text) && !/^\|>/.test(text)) {
            return l;
        }
    }
    return 0;
}

function blockEnd(document, start) {
    let depth = 0;
    const last = document.lineCount - 1;
    for (let l = start; l <= last; l++) {
        const text   = document.lineAt(l).text;
        const opens  = (text.match(/{/g)  || []).length;
        const closes = (text.match(/}/g) || []).length;
        depth += opens - closes;
        if (depth <= 0 && l > start && (opens > 0 || closes > 0)) {
            return l;
        }
        // Single-line construct: check whether next line continues with |> or {
        if (l === start && depth === 0 && l < last) {
            const next = document.lineAt(l + 1).text;
            if (/^\|>/.test(next) || /^\s*{/.test(next)) {
                continue;
            }
            return l;
        }
    }
    return last;
}

function getBlock(document, lineNum) {
    const start = blockStart(document, lineNum);
    const end   = blockEnd(document, start);
    const lines = [];
    for (let l = start; l <= end; l++) lines.push(document.lineAt(l).text);
    return { start, end, code: lines.join('\n') };
}

// Extract "name" from "process name: {" or "process name, dur=...: {"
function processNameAt(document, lineNum) {
    const start = blockStart(document, lineNum);
    const text  = document.lineAt(start).text;
    const m     = text.match(/^process\s+(\w+)/);
    return m ? m[1] : null;
}

// ── Flash decoration ──────────────────────────────────────────────────────────

let flashDecoration = null;

function ensureDecoration() {
    if (!flashDecoration) {
        flashDecoration = vscode.window.createTextEditorDecorationType({
            backgroundColor: new vscode.ThemeColor('editor.findMatchHighlightBackground'),
            isWholeLine: false,
        });
    }
}

function flashRange(editor, startLine, endLine) {
    ensureDecoration();
    const range = new vscode.Range(
        startLine, 0,
        endLine, editor.document.lineAt(endLine).text.length
    );
    editor.setDecorations(flashDecoration, [{ range }]);
    setTimeout(() => {
        if (flashDecoration) editor.setDecorations(flashDecoration, []);
    }, 250);
}

// ── Commands ──────────────────────────────────────────────────────────────────

function cmdEval() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'idyl') return;

    const { start, end, code } = getBlock(editor.document, editor.selection.active.line);
    flashRange(editor, start, end);
    sendOsc('/idyl/eval', code);
    vscode.window.setStatusBarMessage(`[idyl] eval ${start + 1}:${end + 1}  ${editor.document.lineAt(start).text.trim()}`, 3000);
}

function cmdStart() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'idyl') return;

    const name = processNameAt(editor.document, editor.selection.active.line);
    if (!name) {
        vscode.window.showWarningMessage('[idyl] cursor is not inside a named process block');
        return;
    }
    sendOsc('/idyl/process/start', name);
    vscode.window.setStatusBarMessage(`[idyl] start ${name}`, 3000);
}

function cmdStop() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'idyl') return;

    const name = processNameAt(editor.document, editor.selection.active.line);
    if (!name) {
        vscode.window.showWarningMessage('[idyl] cursor is not inside a named process block');
        return;
    }
    sendOsc('/idyl/process/stop', name);
    vscode.window.setStatusBarMessage(`[idyl] stop ${name}`, 3000);
}

// ── Activation ────────────────────────────────────────────────────────────────

function activate(context) {
    context.subscriptions.push(
        vscode.commands.registerCommand('idyl.eval',         cmdEval),
        vscode.commands.registerCommand('idyl.startProcess', cmdStart),
        vscode.commands.registerCommand('idyl.stopProcess',  cmdStop),
    );
}

function deactivate() {
    if (flashDecoration) {
        flashDecoration.dispose();
        flashDecoration = null;
    }
}

module.exports = { activate, deactivate };

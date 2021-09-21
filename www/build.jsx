require('log-timestamp');
const fs = require('fs');
const dirTree = require("directory-tree");
const chokidar = require('chokidar');
const HTMLUglify = require('html-minifier').minify;
const CSSUglify = require('uglifycss');
const JSUglify = require("uglify-js");
const sass = require('compile-sass');
const tsNode = require('ts-node');

var watch = false;

process.argv.forEach(argv =>
{
    if (argv == "--watch") { watch = true; }
});

if (watch)
{
    console.log("Watching files...");
    var watcher = chokidar.watch("./", {persistent: true});
    watcher.on('add', Change);
    watcher.on('change', Change);
    watcher.on('unlink', Unlink);
    watcher.on('error', (error) => { console.error('Error happened', error); });
}
else
{
    console.log("Moving files...");
    const tree = dirTree('./');
    tree.children.forEach(child => { Change(child.path); });
}

function Change(path)
{
    if (path.includes("node_modules")) { return; }

    try
    {
        var pathSplit = path.split("\\");
        var last = pathSplit[pathSplit.length - 1];
    
        if (
            !last.endsWith("build.jsx") &&
            (last.endsWith(".html") ||
            last.endsWith(".css") ||
            last.endsWith(".js")) ||
            last.endsWith(".scss") ||
            last.endsWith(".ts")
        )
        {
            console.log(`Change detected at: '${path}'`);

            var newPath = "..\\data\\" + pathSplit.join("\\");

            fs.readFile(path, 'utf8', (err, data) =>
            {
                if (err) { console.error(err); return; }

                if (!fs.existsSync("..\\data\\" + pathSplit.slice(0, pathSplit.length - 1).join("\\")))
                {
                    fs.mkdirSync("..\\data\\" + pathSplit.slice(0, pathSplit.length - 1).join("\\"), { recursive: true });
                }

                if (last.endsWith(".html"))
                {
                    fs.writeFile(newPath, HTMLUglify(data, { collapseWhitespace: true }), (_err, _data) => { if (err) { console.log(_err); } });
                }
                else if (last.endsWith(".css"))
                {
                    fs.writeFile(newPath, CSSUglify.processString(data), (_err, _data) => { if (err) { console.log(_err); } });
                }
                else if (last.endsWith(".js"))
                {
                    fs.writeFile(newPath, JSUglify.minify(data).code, (_err, _data) => { if (err) { console.log(_err); } });
                }
                //Having issues with this bit right now so I will run the default tasks in their own instances instead of running it through here.
                // else if (last.endsWith(".scss"))
                // {
                //     sass.compileSass(path);
                // }
                // else if (last.endsWith(".ts"))
                // {
                //     tsNode.create({ project: "./tsconfig.json", }).compile(data, last);
                // }
            });
        }
    }
    catch (ex)
    {
        console.error(ex);
    }
}

function Unlink(path)
{
    if (path.includes("node_modules")) { return; }

    try
    {
        var pathSplit = path.split("\\");
        var last = pathSplit[pathSplit.length - 1];
    
        if (
            !last.endsWith("build.jsx") &&
            (last.endsWith(".html") ||
            last.endsWith(".css") ||
            last.endsWith(".js")) ||
            last.endsWith(".scss") ||
            last.endsWith(".ts")
        )
        {
            console.log(`Unlink detected at: '${path}'`);

            var newPath = "..\\data\\" + pathSplit.slice(0, pathSplit.length - 1).join("\\")

            if (fs.existsSync(newPath))
            {
                fs.unlinkSync(newPath + "\\" + last);

                // if (fs.readdirSync(newPath).length == 0)
                // {
                //     fs.rmdirSync(newPath);
                // }
            }
        }
    }
    catch (ex)
    {
        console.error(ex);
    }
}

//Recursively delete all emprt folders.
// function DeleteEmptyFoldersRecursive(path)
// {
//     if (fs.existsSync(path))
//     {
//         fs.readdirSync(path).forEach(function(file, index)
//         {
//             var curPath = path + "\\" + file;
//             if (fs.statSync(curPath).isDirectory())
//             {
//                 DeleteEmptyFoldersRecursive(curPath);
//             }
//             else
//             {
//                 fs.unlinkSync(curPath);
//             }
//         });
//         fs.rmdirSync(path);
//     }
// }
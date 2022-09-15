import pyjs
import tarfile
import io
import json


def set_emscripten_module_locate_file_base_url(base_url):

    pyjs.js.Function(
        f"""
    globalThis.EmscriptenForgeModule['locateFile'] = function(f){{
        let path =  "{base_url}/" + f;
        return path
    }}
    """
    )()


def make_js_func(py_func):
    jspy = pyjs.JsValue(py_func)
    f = jspy.py_call.bind(jspy)
    return f, jspy


def make_js_array(arr):
    js_arr = pyjs.js_array()
    for url in arr:
        js_arr.push(url)
    return js_arr


_parallel_fetch_array_buffer = pyjs.js.Function(
    """
return async function(urls){
    let promises = urls.map(url => fetch(url).then(response => response.arrayBuffer()));
    return await Promise.all(promises);
}
"""
)()

_parallel_fetch_arraybuffers_with_progress_bar = pyjs.js.Function(
    """
return async function(urls, callback){
    
    if(callback===undefined || callback===null)
    {   

        let futures = urls.map((url) => {
            return fetch(url).then(response => response.arrayBuffer());
        })
        return await Promise.all(futures);
    }

    
    async function fetch_arraybuffer_with_progress_bar(url,index, report_total_length,report_progress, report_finished){
        let response = await fetch(url);
        const reader = response.body.getReader();

        // Step 2: get total length
        const contentLength = +response.headers.get('Content-Length');

        report_total_length(index, contentLength);

        // Step 3: read the data
        let receivedLength = 0; // received that many bytes at the moment
        let chunks = []; // array of received binary chunks (comprises the body)
        while(true) {
            const {done, value} = await reader.read();

            if (done) {
                report_finished(index);
                break;
            }
            chunks.push(value);
            receivedLength += value.length;

            report_progress(index, receivedLength);
        }

        // Step 4: concatenate chunks into single Uint8Array
        let chunksAll = new Uint8Array(receivedLength); // (4.1)
        let position = 0;
        for(let chunk of chunks) {
            chunksAll.set(chunk, position); // (4.2)
            position += chunk.length;
        }

        return chunksAll
    }

    let n_urls = urls.length;
    let receivedArr = Array(n_urls).fill(0);
    let totalArr = Array(n_urls).fill(0);
    let finishedArr = Array(n_urls).fill(0);

    function on_progress(){
        let total = totalArr.reduce((partialSum, a) => partialSum + a, 0);
        let recived = receivedArr.reduce((partialSum, a) => partialSum + a, 0);
        let n_finished = finishedArr.reduce((partialSum, a) => partialSum + a, 0);
        
        if(callback !== undefined){
            callback(recived,total,n_finished, n_urls);
        }
    }
    
    function report_finished(index){
        finishedArr[index] = 1;
        on_progress();
    }
         
    function report_total_length(index, total){
        totalArr[index] = total;
        on_progress();
    }
    function report_progress(index, p){
        receivedArr[index] = p;
        on_progress();
    }
    
    let futures = urls.map((url, index) => {
        return fetch_arraybuffer_with_progress_bar(url,index, report_total_length,report_progress, report_finished)
    })
    return await Promise.all(futures);
}
"""
)()


async def parallel_fetch_bytes(urls, callback=None):
    js_urls = make_js_array(urls)
    if callback is None:
        return pyjs.to_py(await _parallel_fetch_array_buffer(js_urls))
    else:
        j_callback, handle = make_js_func(callback)
        result = pyjs.to_py(
            await _parallel_fetch_arraybuffers_with_progress_bar(js_urls, j_callback)
        )
        handle.delete()
        return result


async def parallel_fetch_tarfiles(urls, callback=None):
    arrays = await parallel_fetch_bytes(urls, callback)
    return [tarfile.open(fileobj=io.BytesIO(array), mode="r:bz2") for array in arrays]


async def parallel_fetch_jsons(urls, callback=None):
    arrays = await parallel_fetch_bytes(urls, callback)
    return [json.load(io.BytesIO(array)) for array in arrays]


async def parallel_imports(urls):
    js_urls = make_js_array(urls)
    promises = [pyjs.async_import_javascript(url) for url in urls]
    await pyjs.js.Promise.all(make_js_array(promises))

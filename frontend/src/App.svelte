<script>
import { onMount } from "svelte";
import bops from 'bops'
import HexView from './HexView.svelte'
import DevmemServer from './DevmemServer.js'

let server = null
let inputs = {
	offset: '0x1C0E0F0',
	end: '0x1C0E0F4'
}
let hexViewConfiguration = {
	data: new Uint8Array([0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,0x00, 0x01, 0x02, 0x03, 0xFF,]),
	offset: 0
}

window.debugSend = function (type, data) {
	server.send(type, 0, data)
}

onMount(async () => {
	server = new DevmemServer()
	await server.connect('ws://10.10.10.147:8080')
})

async function changeMemoryArea () {
	const offset = parseInt(inputs.offset)
	await server.changeMemoryArea(offset, parseInt(inputs.end))
	hexViewConfiguration.offset = offset
	await getMemoryData()
}

async function getMemoryData () {
	hexViewConfiguration.data = await server.getMemoryData(0, parseInt(inputs.end) - parseInt(inputs.offset))
}

</script>

<main>
	Address: <input type="text" bind:value={inputs.offset}/> - <input type="text" bind:value={inputs.end}/> <button on:click={e => changeMemoryArea()}>Get</button><br/>
	<HexView data={hexViewConfiguration.data} offset={hexViewConfiguration.offset}/>
</main>

<style>

</style>
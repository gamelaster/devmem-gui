import bops from 'bops'
import Server from './Server.js'

class DevmemServer extends Server
{
  async changeMemoryArea (addressStart, addressEnd) {
    let arr = new Uint8Array(8)
    bops.writeUInt32LE(arr, addressStart, 0)
    bops.writeUInt32LE(arr, addressEnd - addressStart, 4)
    const data = this.sendAsync(1, arr, 2000)
    return data
  }
  async getMemoryData (address, length) {
    let arr = new Uint8Array(8)
    bops.writeUInt32LE(arr, address, 0)
    bops.writeUInt32LE(arr, length, 4)
    const data = this.sendAsync(2, arr, 2000)
    return data
  }
  async getPageSize () {
    return await this.sendAsync(0, [], 1000)
  }
}

export default DevmemServer
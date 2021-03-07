import bops from 'bops'
export class ServerError extends Error
{
  constructor (code, message) {
    super(message)
    this.code = code
  }
}

class Server {
  constructor () {
    this._ws = null
    this._lastMessageId = 0
    this._messagesResolves = []
    this._events = {}
  }

  connect (url) {
    return new Promise((resolve, reject) => {
      this._ws = new WebSocket(url)
      this._ws.onopen = (event) => {
        this._ws.onerror = undefined
        this._ws.onmessage = async event => {
          const data = new Uint8Array(await event.data.arrayBuffer())
          const messageId = data[1]
          if (this._messagesResolves[messageId] !== undefined) {
            const errorCode = data[2]
            clearTimeout(this._messagesResolves[messageId].timeoutId)
            if (errorCode === 0) {
              this._messagesResolves[messageId].resolve(bops.subarray(data, 7))
            } else {
              this._messagesResolves[messageId].reject(new ServerError('Error Code ' + errorCode, errorCode))
            }
            delete this._messagesResolves[messageId]
          }
        }
        resolve()
      }
      this._ws.onerror = function (event) {
        reject()
      }
    })
  }

  send (type, messageId, data) {
    let header = new Uint8Array(6)
    header[0] = type
    header[1] = messageId
    bops.writeUInt32LE(header, data.length, 2)
    if (data instanceof Array) {
      data = new Uint8Array(data)
    }
    const buf = bops.join([header, data])
    this._ws.send(buf)
  }

  sendAsync (type, data, timeout) {
    return new Promise((resolve, reject) => {
      const messageId = this._lastMessageId
      this.send(type, messageId, data)
      const timeoutId = setTimeout(() => {
        reject(new ServerError(`Message ${messageId} timed out!`, 1))
        delete this._messagesResolves[messageId]
      }, timeout)
      this._messagesResolves[messageId] = {
        resolve, reject, timeoutId
      }
      this._lastMessageId = (this._lastMessageId + 1) & 0xFF
    })
  }
}

export default Server
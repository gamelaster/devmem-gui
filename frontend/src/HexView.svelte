<script>
  export let data
  export let offset

  function intToHex (value, leadingZeroes) {
    value = value.toString(16).toUpperCase()
    return value.padStart(leadingZeroes, '0')
  }
</script>
<div class="hex-view">
  {#each {length: Math.ceil(data.length / 16)} as _, i}
  <div class="row">
    <div class="column column-offset">{intToHex(offset + (i * 16), 8)}</div>
    {#each {length: 16} as _, j}
      {#if data[(i*16) + j] !== undefined}
        <div class="column column-data">{intToHex(data[(i*16) + j], 2)}</div>
      {/if}
    {/each}
  </div>
  {/each}
</div>
<style lang="scss">
.hex-view {
  font-family: 'Consolas';
  .row {
    .column {
      display: inline-block;
      &.column-offset {
        width: 80px;
      }
      &.column-data {
        width: 19px;
        margin-right: 5px;
      }
    }
  }
}
</style>
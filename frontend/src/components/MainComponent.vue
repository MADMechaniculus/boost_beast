<template>
  <v-container style="font-family: 'pt_monoregular', monospace">
    <h1>Состояние каналов</h1>
    <v-item-group multiple>
      <v-container>
        <v-row>
          <v-col v-for="item in channels" :key="item.id" cols="12" md="6">
            <v-item>
              <v-card
                :color="item.data.available ? 'dark' : 'error'"
                class="align-center"
                dark
                min-height="100"
                :loading="item.inUpdate ? 'warning' : ''"
              >
                <v-card-title>
                  {{ item.data.cardName }}
                  <v-spacer></v-spacer>
                  <v-btn icon fab :loading="item.inUpdate ? true : false">
                    <v-icon dark v-on:click="reloadDevice(item.id)">
                      mdi-reload
                    </v-icon>
                  </v-btn>
                </v-card-title>
                <v-divider></v-divider>
                <v-card-text v-if="item.data.available">
                  <v-simple-table dense>
                    <template v-slot:default>
                      <thead>
                        <tr>
                          <th>Param</th>
                          <th>Value</th>
                        </tr>
                      </thead>
                      <tbody>
                        <tr>
                          <td>ID</td>
                          <td>{{ item.id }}</td>
                        </tr>
                        <tr>
                          <td>Config</td>
                          <td>{{ item.data.config }}</td>
                        </tr>
                        <tr>
                          <td>Freq</td>
                          <td>{{ getFreq(item.data.freq) }}</td>
                        </tr>
                      </tbody>
                    </template>
                  </v-simple-table>
                </v-card-text>
                <v-card-text v-else>
                  <p>
                    Module error: <b>{{ item.data.errorStr }}</b>
                  </p>
                </v-card-text>
              </v-card>
            </v-item>
          </v-col>
        </v-row>
      </v-container>
    </v-item-group>
    <v-divider></v-divider>
    <h1>Параметры системы</h1>
    <v-container>
      <v-card>
        <v-card-text> Mode: transmitter </v-card-text>
      </v-card>
    </v-container>
  </v-container>
</template>

<script>
import axios from "axios";

export default {
  name: "MainComponent",

  data: () => ({
    channels: [
      {
        id: 0,
        inUpdate: false,
        data: {
          available: true,
          cardName: "AD9467",
          config: 6,
          freq: 12e6,
          errorStr: null,
        },
      },
      {
        id: 1,
        inUpdate: false,
        data: {
          available: true,
          cardName: "AD9467",
          config: 6,
          freq: 12e6,
          errorStr: null,
        },
      },
      {
        id: 2,
        inUpdate: true,
        data: {
          available: false,
          cardName: "AD9467",
          config: 6,
          freq: 12e6,
          errorStr: "Error on init",
        },
      },
      {
        id: 3,
        inUpdate: false,
        data: {
          available: true,
          cardName: "AD9467",
          config: 6,
          freq: 12e6,
          errorStr: null,
        },
      },
    ],
  }),
  methods: {
    getFreq: function (freqValue) {
      if (freqValue <= 1000) {
        return freqValue + " Hz";
      } else {
        return freqValue / 1000 + " MHz";
      }
    },
    reloadDevice: function (id) {
      let counter = 0;
      for (let item of this.channels) {
        if (item.id === id) {
          this.channels[counter].inUpdate = true;

          axios
            .post("localhost:20000/", {
              requestedFuncIndex: 1,
              cardId: id,
            })
            .then((Response) => {
              console.log(Response);
            })
            .catch((err) => {
              console.log(err);
            })
            .finally(() => {
              console.log("Post finish!");
            });

          setTimeout(
            function () {
              this.channels[counter].inUpdate = false;
              this.channels[counter].data.available = true;
            }.bind(this, counter),
            2500
          );
          break;
        } else {
          counter++;
        }
      }
    },
  },
};
</script>
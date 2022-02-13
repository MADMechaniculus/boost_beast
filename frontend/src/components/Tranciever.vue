<template>
  <v-container>
    <v-row>
      <v-col class="text-center">
        <v-row align="center" justify="center">
          <v-btn
            :block="true"
            class="ma-2"
            v-on:click="requestCardLoading = !requestCardLoading"
          >
            Send emergency signal
          </v-btn>
        </v-row>
        <v-row align="center" justify="center">
          <v-btn v-on:click="getAppDescription" :block="true" class="ma-2">
            Get application description
          </v-btn>
        </v-row>
      </v-col>
      <v-col>
        <v-card :loading="requestCardLoading">
          <div>
            <v-card-title>Device info</v-card-title>
            <v-card-text v-if="requestSuccess">
              <pre>{{ getResponse }}</pre>
            </v-card-text>
            <v-card-text v-else>
              <v-alert type="error">ERROR</v-alert>
            </v-card-text>
          </div>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script>
import axios from "axios";

export default {
  name: "Tranciever",

  data: () => ({
    responseData: null,
    requestCardLoading: false,
    requestSuccess: false,
    requestSended: false,
  }),

  methods: {
    getAppDescription: function () {
      this.requestCardLoading = true;
      this.requestSended = true;
      axios
        .post("/", {
          requestedFuncIndex: 0,
        })
        .then((Response) => {
          this.responseData = Response.data;
          console.log(Response.data);
          this.requestSuccess = true;
        })
        .catch((err) => {
          console.log(err);
        })
        .finally(() => {
          console.log("Request ended!");
          this.requestCardLoading = false;
          this.requestSended = false;
        });
    },
  },
  computed: {
    getLoading: function () {
      return this.requestCardLoading;
    },
    getResponse: function () {
      return this.responseData;
    },
  },
};
</script>

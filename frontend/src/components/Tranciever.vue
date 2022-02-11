<template>
  <v-container>
    <v-row>
      <v-col>
        <v-row>
          <v-btn> Send emergency signal </v-btn>
        </v-row>
        <v-row>
          <v-btn v-on:click="getAppDescription">
            Get application description
          </v-btn>
        </v-row>
      </v-col>
      <v-col>
        <v-card loading="requestCardLoading">
          <div v-if="requestSended === true">
            <v-card-title v-if="requestSuccess === true"
              >Device info</v-card-title
            >
            <v-card-text v-else>
              <v-alert type="error">Reques failed!</v-alert>
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
          functionId: 0,
        })
        .then((Response) => {
          console.log(Response.data);
        })
        .catch((err) => {
          console.log(err);
        })
        .finally(() => {
          console.log("Request ended!");
          this.requestCardLoading = false;
        });
    },
  },
};
</script>

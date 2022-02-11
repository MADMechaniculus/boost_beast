<template>
  <v-app>
    <v-app-bar
      app
      color="dark"
      dark
    >
      <div class="d-flex align-center">
        <v-img
          alt="Vuetify Logo"
          class="shrink mr-2"
          contain
          src="https://cdn.vuetifyjs.com/images/logos/vuetify-logo-dark.png"
          transition="scale-transition"
          width="40"
        />

        <v-img
          alt="Vuetify Name"
          class="shrink mt-1 hidden-sm-and-down"
          contain
          min-width="100"
          src="https://cdn.vuetifyjs.com/images/logos/vuetify-name-dark.png"
          width="100"
        />
      </div>

      <v-spacer></v-spacer>

      <v-btn
        text
        v-on:click="selectorFunc"
      >
        <span class="mr-2">Latest Release</span>
        <v-icon>mdi-open-in-new</v-icon>
      </v-btn>
    </v-app-bar>

    <v-main>
      <HelloWorld v-if="selectedTab === 0"/>
      <Tranciever v-if="selectedTab === 1"/>
    </v-main>
  </v-app>
</template>

<script>
import HelloWorld from './components/HelloWorld';
import Tranciever from './components/Tranciever';

export default {
  name: 'App',

  components: {
    HelloWorld,
    Tranciever
  },

  data: () => ({
    selectedTab: 0
  }),
  methods: {

    selectorFunc: function () {
      if (this.selectedTab === 0) {
        this.selectedTab = 1
      } else {
        this.selectedTab = 0
      }
    },

  }
};
</script>
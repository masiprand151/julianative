import { Application, Widget, Column, Row, Text, Button } from "@julia/core";

function Main() {
  return Widget({
    title: "Julia Native",
    width: 500,
    height: 350,

    body: [
      Column({
        spacing: 15,
        padding: 20,

        children: [
          Text({
            text: "Welcome to Julia Native",
            fontSize: 24,
            fontWeight: "bold",
          }),

          Text({
            text: "Native Desktop Framework",
          }),

          Row({
            spacing: 10,

            children: [
              Button({
                text: "Click Me",
                onClick: () => {
                  console.log("Button clicked!");
                },
              }),

              Button({
                text: "Exit",
                onClick: () => {
                  console.log("Exit clicked!");
                },
              }),
            ],
          }),
        ],
      }),
    ],
  });
}

Application.run(Main);

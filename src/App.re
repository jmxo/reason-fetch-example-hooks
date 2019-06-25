open Belt;

type dog = string;

type state =
  | Loading
  | Error
  | Loaded(array(dog));

type action =
  | DogsFetch
  | DogsFetched(array(dog))
  | DogsFailedToFetch;

module Decode = {
  let dogs = (json): array(dog) =>
    Json.Decode.(
      json |> field("message", array(string)) |> Array.map(_, dog => dog)
    );
};

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | DogsFetch => Loading
        | DogsFetched(dogs) => Loaded(dogs)
        | DogsFailedToFetch => Error
        },
      Loading,
    );

  React.useEffect0(() => {
    Js.Promise.(
      Fetch.fetch("https://dog.ceo/api/breeds/list")
      |> then_(Fetch.Response.json)
      |> then_(json =>
           json
           |> Decode.dogs
           |> (dogs => dispatch(DogsFetched(dogs)))
           |> resolve
         )
      |> catch(_err => Js.Promise.resolve(dispatch(DogsFailedToFetch)))
      |> ignore
    );

    None;
  });

  switch (state) {
  | Error => <div> {ReasonReact.string("An error occurred!")} </div>
  | Loading => <div> {ReasonReact.string("Loading...")} </div>
  | Loaded(dogs) =>
    <div>
      <h1> {ReasonReact.string("Dogs")} </h1>
      <p> {ReasonReact.string("Source: ")} </p>
      <a href="https://dog.ceo"> {ReasonReact.string("https://dog.ceo")} </a>
      <ul>
        {Array.map(dogs, dog => <li key=dog> {ReasonReact.string(dog)} </li>)
         |> ReasonReact.array}
      </ul>
    </div>
  };
};

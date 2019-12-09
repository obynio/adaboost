
## Synopsis

**htcpcp** is a facial recognition program entirely written from scratch in C.

![game_of_thrones](https://user-images.githubusercontent.com/2095991/42735748-d089da54-8859-11e8-8f0d-61f3abe50a0c.png)

## Detection

The detection relies on a custom implementation of the Viola & Jones algorithm which provides four main stages :

* Pseudo-Haar features calculation
* Feature selection with adaboost
* Attentional cascading
* Post-processing

## Recognition

The recognition relies on a custom implementation of the Eigenface method.
However, this part is not completely finished and still needs some work.

## Make

You need `sdl`, `sdl_image` and `json-c` libraries. Then build the project using `make`.

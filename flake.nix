{
  description = "Kuriko's Python Template";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";

    devenv.url = "github:cachix/devenv/latest";
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

    kuriko-nur.url = "github:kurikomoe/nur-packages";
    kuriko-nur.inputs.nixpkgs.follows = "nixpkgs";
  };

  nixConfig = {
    substituters = [
      "https://cache.nixos.org"
      "https://nixpkgs-python.cachix.org"
      "https://nix-community.cachix.org"
      "https://mirrors.ustc.edu.cn/nix-channels/store"
      "https://mirrors.tuna.tsinghua.edu.cn/nix-channels/store"
      "https://kurikomoe.cachix.org"
    ];
    trusted-public-keys = [
      "cache.nixos.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY="
      "nixpkgs-python.cachix.org-1:hxjI7pFxTyuTHn2NkvWCrAUcNZLNS3ZAvfYNuYifcEU="
      "nix-community.cachix.org-1:mB9FSh9qf2dCimDSUo8Zy7bkq5CX+/rkCWyvRCYg3Fs="
      "kurikomoe.cachix.org-1:NewppX3NeGxT8OwdwABq+Av7gjOum55dTAG9oG7YeEI="
    ];
    extra-trusted-public-keys = "devenv.cachix.org-1:w1cLUi8dv3hnoSPGAuibQv+f9TZLr6cv/Hm9XgU50cw=";
    extra-substituters = "https://devenv.cachix.org";
  };

  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [inputs.devenv.flakeModule];
      systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin"];
      perSystem = {
        config,
        self',
        inputs',
        system,
        lib,
        ...
      }: let
        pkgs = import inputs.nixpkgs {
          inherit system;
          config.allowUnfree = true;
          overlays = [];
        };

        runtimeLibs = with pkgs; [];
      in {
        devenv.shells.default = {
          packages = with pkgs;
            [
              hello
              rsync

              git
              git-filter-repo

              # protobuf editor
              protoscope

              # Github Action Tester
              act
            ]
            ++ runtimeLibs;

          env = {
            LD_LIBRARY_PATH = lib.makeLibraryPath runtimeLibs;
          };

          enterShell = ''
            hello
          '';

          languages.python = {
            enable = true;
            package = pkgs.python312;
            poetry = {
              enable = true;
              activate.enable = true;
            };
          };

          dotenv.enable = true;
          dotenv.filename = [".env"];

          pre-commit.hooks = {
            alejandra.enable = true;

            isort.enable = true;
            autoflake.enable = true;
            mypy = {
              enable = true;
              excludes = [
                ".*yarn_spinner_pb2.py$"
                "yarn_spinner_pb2.py"
                "third/.*"
              ];
              args = [
                "--disable-error-code=attr-defined"
              ];
              extraPackages = with pkgs; [
                python312Packages.protobuf
                python312Packages.types-protobuf
              ];
            };

            trufflehog = {
              enable = true;
              entry = builtins.toString inputs.kuriko-nur.legacyPackages.${system}.precommit-trufflehog;
              stages = ["pre-push" "pre-commit"];
            };

            # pylint.enable = true;
            # flake8.enable = true;
          };

          cachix.pull = ["devenv"];
          cachix.push = "kurikomoe";
        };
      };

      flake = {};
    };
}

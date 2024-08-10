# Contributing to HarvesterHarvester

Thank you for considering contributing to HarvesterHarvester! We welcome contributions of all kinds, from code to documentation, and we're excited to work with you to improve our project. Here are some guidelines to help you get started.

## How to Contribute

### 1. Clone the Repository

Clone the repository to your local machine using:

```bash
git clone https://github.com/IcePanorama/HarvesterHarvester.git
# or ...
git clone git@github.com:IcePanorama/HarvesterHarvester.git
```

### 2. Create a Branch

Create a new branch for your changes:

```bash
git checkout -b feature/your-feature
```

### 3. Make Changes

Make your changes to the code or documentation. Be sure to follow our coding style and conventions.

We use clang-format for autoformatting. If you are unable to run clang-format on your machine for whatever reason, please follow the [GNU C style conventions](https://www.gnu.org/prep/standards/html_node/Writing-C.html).

Finally, if possible, prior to commiting/pushing your changes run `make full` to test the program using Valgrind first.

### 4. Commit Your Changes

Commit your changes with a descriptive commit message:

```bash
git add .
git commit -m "Add a feature or fix a bug"
```

### 5. Push Your Changes

Push your changes to the repository:

```bash
git push origin feature/your-feature
```

### 6. Submit a Pull Request

Go to the original repository on GitHub and open a pull request. Provide a clear description of the changes you’ve made and any additional context or information that may be helpful for reviewers.

## Code of Conduct

We are committed to providing a welcoming and inclusive community for everyone. Please read and follow our [Code of Conduct]().

## Development Guidelines

* Coding Standards: Follow the coding standards used in this project. Refer to our [style guide]() for more information.

* Commit Messages: Write clear and concise commit messages.

* Documentation: Update documentation if your changes affect existing functionality or introduce new features.

* Testing: If possible, run `make full` prior to commiting/pushing your changes. This will test your program with Valgrind.

## Reporting Issues

If you find a bug or have a feature request, please open an issue in the [issue tracker](https://github.com/IcePanorama/HarvesterHarvester/issues). Provide as much detail as possible to help us understand and address the issue.

## Contact

For any questions or additional information, feel free to reach out to us via email <open-harvester@proton.me> or open a discussion on GitHub.

Thank you for contributing to HarvesterHarvester! Your help is invaluable and appreciated.
